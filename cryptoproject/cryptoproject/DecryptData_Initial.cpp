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
			/*
			//xor byte ptr[edi + ecx], 'B' - Seth
			mov bl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
			mov bh, bl
			shr bh, 4 //upper->lower
			shl bl, 4//lower->upper
			add bl, bh //add lower to upper
			mov byte ptr[edi + ecx], bl //move back to memory
			*/
			//xor byte ptr[edi + ecx], 'D'		//Eddie
			mov al, byte ptr[edi + ecx]
			mov bl, al
			mov dl, al
			and dl, 0xF0
			and bl, 0x0F
			ror dl, 4
			rol bl, 5// left shift 
			jc CARRYSET //  if there is a one proceed to function
			jnc CONTINUE// else continue to next operation

			CARRYSET ://if cf has a one
				or bl, 0x10 //set 5th bit to 1
				xor bl, 0x01//clear lower nibble
				jmp CONTINUE//continue with program
			CONTINUE :
				ror dl, 1 // shift right 
				jc CARRYDL// if cf is set 
				jnc LAST//else finish step 

			CARRYDL:
				or dl, 0x08// set 4th bit to 1
				xor dl, 0x80// clear upper nibble 
				jmp LAST
			LAST:
				rol dl,4 // shift nibbles to upper half
				ror bl,4// shift nibbles to lower half
				add bl, dl// put back together and added it back to the data
				mov byte ptr[edi + ecx], bl

	//xor byte ptr[edi + ecx], 'C'
				mov al, byte ptr[edi + ecx]
				push ecx //store ecx for later use
				xor bl, bl  // set bl to 0x00
				mov cx,8 //set up loop
				CLOOP:
					rcr al, 1
					rcl bl, 1
					loop CLOOP // Do 8 times
				pop ecx //restore ecx 
				mov	byte ptr[edi + ecx], bl
		
				inc ecx
				cmp ecx, dataLength // check to see if we have reached the end of the data file 
				jb Start // jump to start of loop if ecx is smaller than datalength 

	}
	return resulti;
} // decryptData

