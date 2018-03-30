// EncryptData.cpp
//
// This file uses the input data and key information to encrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to encrypt the data as specified by the project assignment
int encryptData(char *data, int dataLength)
{
	int resulti = 0;

	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	int index=0;
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
		mov ebx,2h
		mov al,byte ptr [esi+ebx*2]			// get 5th byte of password hash
		mov al,byte ptr [esi+ebx]			// get 3rd byte of password has

		mov ax,word ptr [esi+ebx*2]			// gets 5th and 6th bytes of password hash ( gPasswordHash[4] and gPasswordHash[5] ) into ax
		mov eax,dword ptr [esi+ebx*2]		// gets 4 bytes, as in:  unsigned int X = *( (unsigned int*) &gPasswordHash[4] );

		mov al,byte ptr [gkey+ebx]			// get's 3rd byte of gkey[] data

		mov al,byte ptr [gptrKey+ebx]		// THIS IS INCORRECT - will add the address of the gptrKey global variable (NOT the value that gptrKey holds)

		mov al,byte ptr [esi+0xd];			// access 14th byte in gkey[]: 0, 1, 2 ... d is the 14th byte
		mov edi,data				// Put ADDRESS of first data element into edi
		xor byte ptr [edi+1],al		// Exclusive-or the 2nd byte of data with the 14th element of the keyfile
									// NOTE: Keyfile[14] = 0x21, that value changes the case of a letter and flips the LSB
									// Capital "B" = 0x42 becomes lowercase "c" since 0x42 xor 0x21 = 0x63
									
	}
	*/
	__asm {// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds
		
		// simple example that xors 2nd byte of data with 14th byte in the key file
		AND ecx, 0 //clear ecx from any residual data from prior operations 
		mov edi, data				// Put ADDRESS of first data element into edi,
		Start : // start of the loop  
		
			//xor byte ptr[edi + ecx], 'A' //Seth		// Exclusive-or byte
				mov bl, byte ptr[edi+ecx] //move data byte to bl (part of ebx) to rotate
				rol bl, 1
				mov byte ptr[edi+ecx], bl
			//xor byte ptr[edi + ecx], 'E'
			
			//xor byte ptr[edi + ecx], 'B' //Seth
				mov bl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
				mov bh, bl
				shr bh, 4 //upper->lower
				shl bl, 4//lower->upper
				add bl, bh //add lower to upper
				mov byte ptr[edi+ecx], bl //move back to memory
			//xor byte ptr[edi + ecx], 'D'
		
			//xor byte ptr[edi + ecx], 'C'
			mov	al, byte ptr[edi + ecx]
			xor bl, bl  // BX to zero
			mov cl, 8
		RLOOP:
			rcr al, 1
			rcl bl, 1
			loop RLOOP // Do 8 times

			mov al, bl
			mov	byte ptr[edi + ecx], al
			
			inc ecx
			cmp ecx, dataLength // check to see if we have reached the end of the data file 
			jb Start // jump to start of loop if ecx is smaller than datalength 
	}
	return resulti;
} // encryptData