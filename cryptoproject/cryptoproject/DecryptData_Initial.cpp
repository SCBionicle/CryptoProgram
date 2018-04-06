// DecryptData.cpp
//
// THis file uses the input data and key information to decrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to decrypt the data as specified by the project assignment
int decryptData(char *data, int dataLength)
{
	int resulti = 0;
	int index = 0;
	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	// You can not declare any local variables in C, but should use resulti to indicate any errors
	// Set up the stack frame and assign variables in assembly if you need to do so
	// access the parameters BEFORE setting up your own stack frame
	// Also, you cannot use a lot of global variables - work with registers
	/*
	__asm {
		// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds

		// simple example that xors 2nd byte of data with 14th byte in the key file
		lea esi,gkey				// put the ADDRESS of gkey into esi
		mov esi,gptrKey;			// put the ADDRESS of gkey into esi (since *gptrKey = gkey)

		lea	esi,gPasswordHash		// put ADDRESS of gPasswordHash into esi
		mov esi,gptrPasswordHash	// put ADDRESS of gPasswordHash into esi (since unsigned char *gptrPasswordHash = gPasswordHash)

		mov al,byte ptr [esi]				// get first byte of password hash
		mov al,byte ptr [esi+4]				// get 5th byte of password hash
		mov ebx,2
		mov al,byte ptr [esi+ebx]			// get 3rd byte of password hash
		mov al,byte ptr [esi+ebx*2]			// get 5th byte of password hash

		mov ax,word ptr [esi+ebx*2]			// gets 5th and 6th bytes of password hash ( gPasswordHash[4] and gPasswordHash[5] ) into ax
		mov eax,dword ptr [esi+ebx*2]		// gets 4 bytes, as in:  unsigned int X = *( (unsigned int*) &gPasswordHash[4] );

		mov al,byte ptr [gkey+ebx]			// get's 3rd byte of gkey[] data

		mov al,byte ptr [gptrKey+ebx]		// THIS IS INCORRECT - will add the address of the gptrKey global variable (NOT the value that gptrKey holds)

		mov al,byte ptr [esi+0xd];			// access 14th byte in gkey[]: 0, 1, 2 ... d is the 14th byte
		mov edi,data				// Put ADDRESS of first data element into edi
		xor byte ptr [edi+1],al		// Exclusive-or the 2nd byte of data with the 14th element of the keyfile
									// NOTE: Keyfile[14] = 0x21, that value changes the case of a letter and flips the LSB
									// Lowercase "c" = 0x63 becomes capital "B" since 0x63 xor 0x21 = 0x42
	}
	*/
	__asm {// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds

		
		AND ecx, 0 //clear ecx from any residual data from prior operations 
			mov edi, data				// Put ADDRESS of first data element into edi,
		Start : // start of the loop  
			//xor byte ptr[edi + ecx], 'A'		// Seth - Exclusive-or byte
			mov bl, byte ptr[edi + ecx] //move data byte to bl (part of ebx) to rotate
			ror bl, 1
			mov byte ptr[edi + ecx], bl
			//xor byte ptr[edi + ecx], 'E'
			//xor byte ptr[edi + ecx], 'B' - Seth
			mov bl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
			mov bh, bl
			shr bh, 4 //upper->lower
			shl bl, 4//lower->upper
			add bl, bh //add lower to upper
			mov byte ptr[edi + ecx], bl //move back to memory

			//xor byte ptr[edi + ecx], 'D'		//Eddie
			mov al, byte ptr[edi + ecx]
			push ecx // store index 
			mov cx, 4// for obtaining nibbles
			xor bl, bl//lower nibble
			xor bh, bh//upper nibble
			DLOOP ://send lower four bits to upper half of bl 
				rcr al, 1
				rcr bl, 1
				loop DLOOP
			mov cx, 4

			DLOOP2://send left over four bits into upper half of bh 
				rcr al, 1
				rcr bh, 1
				loop DLOOP2
			ror bh, 4 //since shift will be right shift to lower nibble
			rol bl, 1// left shift 
			jc CARRYSET //  if there is a one proceed to function
			jnc CONTINUE// else continue to next operation

			CARRYSET ://if cf has a one
				or bl, 0x10 //set 5th bit to 1
				xor bl, 0x01//clear lower nibble
				jmp CONTINUE//continue with program
			CONTINUE :
				ror bh, 1 // shift right 
				jc CARRYBL// if cf is set 
				jnc LAST//else finish step 

			CARRYBL:
				or bh, 0x08// set 4th bit to 1
				xor bh, 0x80// clear upper nibble 
				jmp LAST
			LAST:
				rol bh,4 // shift nibbles to upper half
				ror bl,4// shift nibbles to lower half
				add bl, bh// put back together and added it back to the data
				pop ecx
				mov byte ptr[edi + ecx], bl
		//xor byte ptr[edi + ecx], 'C'
			mov al, byte ptr[edi + ecx]
			xor bl, bl  // bl to zero
			push ecx 
			mov cx,8
			CLOOP:
				rcr al, 1
				rcl bl, 1
				loop CLOOP // Do 8 times
			mov	byte ptr[edi + ecx], bl
			pop ecx
			
			inc ecx
			cmp ecx, dataLength // check to see if we have reached the end of the data file 
			jb Start // jump to start of loop if ecx is smaller than datalength 
	}
	return resulti;
} // decryptData

