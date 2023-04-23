#include "des.h"
#include "desf.h"
	
//lastkey is used to recover the round key of the previous round
//empty lastkey means we are recovering the final round key
string attack( vector<string> rkb, vector<string> rk, string lastkey = "")
{
	string recovered = "";
	//inject each Sbox once keeping others fixed
	//each Sbox recovers a block of 6 bits 
	//after running on all 8 Sboxes, we recover the 48-bit key
	for (int i = 1; i<=8; i++ )
	{
		//Number of attempts to recover a block
		int trial = 0;
		//inject fault in ith Sfault-box 
		sfault[i-1][0][0] = 0;
		do{
			string pt = gen_random();
			string cipher = encrypt(pt, rkb, rk, s);
			string fault = encrypt(pt, rkb, rk, sfault);
			
			trial++;
			if (fault != cipher)
				{
					// reverse final permutation
					cipher = permute(hex2bin (cipher), initial_perm, 64 );
					fault = permute(hex2bin(fault), initial_perm, 64 );
					
					//if we are recovering keys from non-final rounds, decrypt to get the intermediate ciphertexts
					if (lastkey.size())
					{
						vector<string> lkb; 
						vector<string> lk;
						lkb.push_back(lastkey);
						lk.push_back(bin2hex(lastkey)); 
						cipher = encrypt (cipher, lkb, lk, s, 1);
						fault = encrypt (fault, lkb, lk, s, 1);
					}


					// splitting 
					string yl = cipher.substr(0, 32);
					string yr = cipher.substr(32, 32);
					string ylbar = fault.substr(0, 32);
					string yrbar = fault.substr(32, 32);
					
					//left and right is not flipped for last round
					if (lastkey.size())
					{	
						swap (yl, yr);
						swap(ylbar, yrbar);
					}
					
					// reverse straight permutation

					yl = permute(yl, invper, 32 );
					ylbar = permute(ylbar, invper, 32 );
					
					// take xor of correct and faulty CTs
					string a = xor_ (yl, ylbar);
					int temp = s[i-1][0][0];
					string l = "";
					for (int j=0;j<32;j++)
					{
						if (j != 4*(i-1))
							l = l + '0';
						else
						{
							l = l + dec2bin(temp);
							j += 3;
						}
					}
					
					/* the calculated xor value is equal to the original value in the faulty Sbox (temp variable) for its ith 4-bit block and zero for
					   the remaining 7 blocks
					   if the faulty entry has been accessed only in the final round */ 
					if (a == l)
					{
						
						
						//cout << "Plain Text: " << pt << endl;
						//cout << "Cipher Text: " << cipher << endl;
						//cout << "Faulty Text: " << fault << endl;
						
						// input gets expanded in round function
						yr = permute(yr, exp_d, 48);
						
						// ith 6-bit block of yr gives the ith 6-bit block of the key 
						recovered =  recovered + yr.substr((i-1)*6,6) ;
						cout << "Block "<<i<<":	Key = "<< yr.substr((i-1)*6,6)<<"	Trials = "<<trial<<endl;
						break;
					}
			}
			
		} while(true);
		
		//fix the fault
		sfault[i-1][0][0] = s[i-1][0][0];

	}
	cout<<"Recovered Round Key: "<<recovered<<endl;
	return recovered;
}


void mastkerkey(string key1, string key2)
{



	string key1_exp = permute(key1, key_exp, 56);  //ultimate key
	string key2_exp = permute(key2, key_exp, 56);  //penultimate key



	string left1 = key1_exp.substr(0, 28); 		//splitting
	string right1 = key1_exp.substr(28, 28); 

	string left2 = key2_exp.substr(0, 28); 
	string right2 = key2_exp.substr(28, 28); 
	
	left2 = shift_left(left2,1);			//traceback
	right2 = shift_left(right2,1);

	string ultimate = left1+right1;
	string penultimate = left2+right2;

	cout<<"Extracted Master key: "<<bin2hex (or_(ultimate, penultimate))<<endl;	//master key of 56bit 

}

int main() 
{ 

	srand (time(0));
	string key;
	vector<string> rkb; // rkb for RoundKeys in binary 
	vector<string> rk; // rk for RoundKeys in hexadecimal 
	

	//key = "AABB09182736CCDD"; 
	cout<<"Enter 64-bit key in hexadecimal (eg. AABB09182736CCDD): ";
	cin>> key;
	string key_without_parity = key_schedule(key, rkb, rk);
	//cout<<"Given 64-bit key: "<<key<<endl;
	cout<<"56-bit key after parity drop: "<<key_without_parity<<endl;
	cout<<"Round keys for each Round:"<<endl;
	for (int i = 0; i<rk.size(); i++)
		cout<<"48-bit key for Round "<<i+1<<": "<<rk[i]<<endl;
	
	
	cout<<endl<<"After PFA Key Recovery "<<endl;
	string rk15 = "";
	cout<<"Ultimate Round:"<<endl;
	rk15 = attack (rkb, rk);
	cout<<"Ultimate key:"<<bin2hex(rk15)<<endl;
	cout<<"Penultimate Round:"<<endl;
	string rk14 = attack (rkb, rk, rk15);
	cout<<"Penultimate key:"<<bin2hex(rk14)<<endl;
	mastkerkey(rk15, rk14);
	
} 
