#include "des.h"




string encrypt(string pt, vector<string> rkb, vector<string> rk, int s[M][N][P] ) 
{ 
	pt = permute(hex2bin(pt), initial_perm, 64); // Initial Permutation 
	cout << "After initial permutation: " << bin2hex(pt) << endl;
	// Splitting 
	string left = pt.substr(0, 32); 
	string right = pt.substr(32, 32); 
	cout << "After splitting: Left=" << bin2hex(left) << " Right=" << bin2hex(right) << endl; 
	cout << "  	  Left	"<<" Right	"<<" Key	"<<endl;
	for (int i = 0; i < rkb.size(); i++) { 
		string right_expanded = permute(right, exp_d, 48);  // Expansion D-box
		string x = xor_(rkb[i], right_expanded);  // XOR RoundKey[i] and right_expanded
		// S-boxes 
		string op = ""; 
		for (int i = 0; i < 8; i++) { 
			int row = 2 * int(x[i * 6] - '0') + int(x[i * 6 + 5] - '0'); 
			int col = 8 * int(x[i * 6 + 1] - '0') + 4 * int(x[i * 6 + 2] - '0') + 2 * int(x[i * 6 + 3] - '0') + int(x[i * 6 + 4] - '0'); 
			int val = s[i][row][col]; 
			op += char(val / 8 + '0'); 
			val = val % 8; 
			op += char(val / 4 + '0'); 
			val = val % 4; 
			op += char(val / 2 + '0'); 
			val = val % 2; 
			op += char(val + '0'); 
		} 
		op = permute(op, per, 32); // Straight D-box 
		x = xor_(op, left); // XOR left and op 
		left = x; 
		if (i != rkb.size()-1) 
			swap(left, right); // Swapper 
		cout << "Round " << i + 1 << ": " << bin2hex(left) << " " << bin2hex(right) << " " << rk[i] << endl; 
	} 
	string cipher = left + right; // Combination 
	cipher = bin2hex(permute(cipher, final_perm, 64)); // Final Permutation
	cout << "After final permutation: " << cipher << endl;
	return cipher; 
} 



string key_schedule(string key, vector<string> &rkb, vector<string>& rk)
{
	cout<<"Given 64-bit key: "<<key<<",";
	key = hex2bin(key);  
 
	key = permute(key, keyp, 56); // key without parity 

	string key_without_parity = bin2hex(key); 

	// Splitting 
	string left = key.substr(0, 28); 
	string right = key.substr(28, 28); 

 
	for (int i = 0; i < 16; i++) { 
		// Shifting 
		left = shift_left(left, shift_table[i]); 
		right = shift_left(right, shift_table[i]); 

		// Combining 
		string combine = left + right; 

		// Key Compression 
		string RoundKey = permute(combine, key_comp, 48); 

		rkb.push_back(RoundKey); 
		rk.push_back(bin2hex(RoundKey)); 
	} 
	
	cout<<"	56-bit key after parity drop: "<<key_without_parity<<endl;
	return key_without_parity;
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
	string pt = gen_random();
	cout<<"Plain Text: "<<pt<<endl;
	cout<<"Encryption:"<<endl;
	string cipher = encrypt(pt, rkb, rk, s);
	reverse(rkb.begin(), rkb.end());
	reverse(rk.begin(), rk.end());
	cout<<"Decryption:"<<endl;
	encrypt(cipher, rkb, rk, s);
 
}

