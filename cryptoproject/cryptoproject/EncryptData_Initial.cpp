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
 
	int index = 0;
	int iNumOfRounds = 0;
	// You can not declare any local variables in C, but should use resulti to indicate any errors
	// Set up the stack frame and assign variables in assembly if you need to do so
	// access the parameters BEFORE setting up your own stack frame
	// Also, you cannot use a lot of global variables - work with registers

	__asm {// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds

		// simple example that xors 2nd byte of data with 14th byte in the key file
		mov edi, data// Put ADDRESS of first data element into edi,
		SetIndex : // setup for obtaining index from PassHash, and hop count
			xor ecx, ecx //clear registers from any residual data from prior operations 
			xor edx, edx
			xor ebx, ebx
			xor eax, eax
			lea esi, gPasswordHash //grap pointer 
			mov edx, iNumOfRounds // self explanatory
			//starting index, index = PasswordHash[rounds*4 + i]
			mov ah, [esi + edx * 4]
			mov al, [esi + edx * 4 + 1]
			mov [esp-8],eax
			//hop count , hopCount = 
			mov bh, [esi + edx * 4 + 2]
			mov bl, [esi + edx * 4 + 3]
			mov [esp-4], ebx
			cmp bx, 0
			jne Hopping
			mov bx, 0xFFFF
			mov[esp - 4], ebx
			jmp Hopping
		Hop:
			mov eax, [esp - 8]//load index 
			mov ebx, [esp - 4]//load hop count
		Hopping:
			xor edx, edx
			lea esi, gkey//load global key into esi
			mov dl, byte ptr[edi + ecx]
			xor dl, byte ptr[esi + eax]
			mov byte ptr[edi + ecx], dl
			add eax, ebx //add hop count to starting point
			cmp eax, 65537//cmp to max 
			jb Start// if below 
			sub eax, 65537 // if greater subtract by this num
			jmp Start //encryting steps
		
		Rounds://check to see if there are more rounds to go 
			inc iNumOfRounds
			mov edx, gNumRounds
			cmp iNumOfRounds, edx
			je Done
			jne SetIndex
		
		Start : // start of the loop 	
			mov[esp - 8], eax//save next index to xor with into stack
			xor eax, eax 
		//xor byte ptr[edi + ecx], 'A' //Seth		// Exclusive-or byte
			mov bl, byte ptr[edi + ecx] //move data byte to bl (part of ebx) to rotate
			rol bl, 1
			mov byte ptr[edi + ecx], bl
		//xor byte ptr[edi + ecx], 'E'MAX
			mov dl, byte ptr[edi + ecx]
			lea  esi, gEncodeTable
			movzx edx, dl
			mov dl, byte ptr[esi + edx]
			mov byte ptr[edi + ecx], dl
		//xor byte ptr[edi + ecx], 'B' //Seth 
			mov bl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
			mov bh, bl
			shr bh, 4 //upper->lower
			shl bl, 4//lower->upper
			add bl, bh //add lower to upper
			mov byte ptr[edi + ecx], bl //move back to memory
		//xor byte ptr[edi + ecx], 'D'
			mov al, byte ptr[edi + ecx] // grab byte from data
			mov bl, al 
			mov dl, al
			and bl, 0x0F//clear higher nibble 0000-xxxx
			and dl, 0xF0//clear lower nibble  xxxx-0000
			rol dl, 1// xxx0-000x
			jc CARRYSET
			jnc CONTINUE
		CARRYSET :
			or dl, 0x10 //set 5th bit to 1 xxx1-0001
			xor dl, 0x01//set first bit to 0 xxx1-0000
			jmp CONTINUE
		CONTINUE :
			ror bl, 1// x000-0xxx
			jc CARRYBL
			jnc LAST
		CARRYBL :
			or bl, 0x08// set 4th bit to 1 1000-1xxx
			xor bl, 0x80//set 7th bit to 0 0000-1xxx
			jmp LAST
		LAST :
			add dl, bl// set back together xxxx-xxxx
			mov byte ptr[edi + ecx], dl

		//xor byte ptr[edi + ecx], 'C' 
			mov al, byte ptr[edi + ecx]
			xor dl, dl  // set bl to 0x00
			xor bl, bl
		CLOOP :
			rcr al, 1
			rcl dl, 1
			inc bl
			cmp bl, 8
			jne CLOOP // Do 8 times
			mov	byte ptr[edi + ecx], dl
			inc ecx
			cmp ecx, dataLength // check to see if we have reached the end of the data file 
			jb HOP //go onto next hop 
			jmp Rounds// check to see if we need to do another round of encryption
		DONE:
			
	}
	return resulti;
} // encryptData