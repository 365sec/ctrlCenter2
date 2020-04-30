#ifndef _BASE64_H_
#define _BASE64_H_

#define BASE64_FLAG_NONE        0
#define BASE64_FLAG_NOPAD       1
#define BASE64_FLAG_NOCRLF		2

inline int Base64EncodeGetRequiredLength(int nSrcLen, unsigned dwFlags = BASE64_FLAG_NONE)
{
        int nRet = nSrcLen*4/3;

        if((dwFlags & BASE64_FLAG_NOPAD) == 0)
                nRet += nSrcLen % 3;

        int nCRLFs = nRet / 76 + 3;
        int nOnLastLine = nRet % 76;

        if(nOnLastLine) {
                if(nOnLastLine % 4)
                        nRet += 4 - (nOnLastLine % 4);
        }

        nCRLFs *= 2;

        if((dwFlags & BASE64_FLAG_NOCRLF) == 0)
                nRet += nCRLFs;

        return nRet;
}

inline int Base64DecodeGetRequiredLength(int nSrcLen)
{
        return nSrcLen;
}

inline bool Base64Encode(
        const unsigned char *pbSrcData,
        int nSrcLen,
        char * szDest,
        int * pnDestLen,
        unsigned dwFlags = BASE64_FLAG_NONE)
{
        static const char s_chBase64EncodingTable[64] = {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
                'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
                'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

        if (!pbSrcData || !szDest || !pnDestLen)
                return false;

        int nWritten(0);
        int nLen1((nSrcLen / 3) * 4);
        int nLen2(nLen1 / 76);
        int nLen3(19);
        int i, j, k, n;

        for(i = 0; i <= nLen2; i++) {
                if(i == nLen2)
                        nLen3 = (nLen1 % 76) / 4;

                for(j = 0; j < nLen3; j++) {
                        unsigned dwCurr(0);
                        for(int n = 0; n < 3; n++)      {
                                dwCurr |= *pbSrcData++;
                                dwCurr <<= 8;
                        }
                        for(k = 0; k < 4; k++) {
                                byte b = (byte)(dwCurr >> 26);
                                *szDest++ = s_chBase64EncodingTable[b];
                                dwCurr <<= 6;
                        }
                }
                nWritten += nLen3 * 4;

                if((dwFlags & BASE64_FLAG_NOCRLF) == 0) {
                        *szDest++ = '\r';
                        *szDest++ = '\n';
                        *szDest++ = '\t';               // as vcards have tabs in second line of binary data
                        nWritten += 3;
                }
        }

        if (nWritten && (dwFlags & BASE64_FLAG_NOCRLF) == 0) {
                szDest -= 2;
                nWritten -= 2;
        }

        nLen2 = nSrcLen % 3 ? nSrcLen % 3 + 1 : 0;
        if(nLen2) {
                unsigned dwCurr(0);
                for (n = 0; n < 3; n++)
                {
                        if (n < (nSrcLen % 3))
                                dwCurr |= *pbSrcData++;
                        dwCurr <<= 8;
                }
                for(k = 0; k < nLen2; k++) {
                        byte b = (byte)(dwCurr >> 26);
                        *szDest++ = s_chBase64EncodingTable[b];
                        dwCurr <<= 6;
                }
                nWritten+= nLen2;
                if((dwFlags & BASE64_FLAG_NOPAD) == 0) {
                        nLen3 = nLen2 ? 4 - nLen2 : 0;
                        for (j = 0; j < nLen3; j++)     {
                                *szDest++ = '=';
                        }
                        nWritten+= nLen3;
                }
        }

        *pnDestLen = nWritten;
        return true;
}

inline int DecodeBase64Char(unsigned int ch)
{
	// returns -1 if the character is invalid
	// or should be skipped
	// otherwise, returns the 6-bit code for the character
	// from the encoding table
	if (ch >= 'A' && ch <= 'Z')
		return ch - 'A' + 0;	// 0 range starts at 'A'
	if (ch >= 'a' && ch <= 'z')
		return ch - 'a' + 26;	// 26 range starts at 'a'
	if (ch >= '0' && ch <= '9')
		return ch - '0' + 52;	// 52 range starts at '0'
	if (ch == '+')
		return 62;
	if (ch == '/')
		return 63;
	return -1;
}

inline bool Base64Decode(const char * szSrc, int nSrcLen, unsigned char *pbDest, int *pnDestLen)
{
	// walk the source buffer
	// each four character sequence is converted to 3 bytes
	// CRLFs and =, and any characters not in the encoding table
	// are skiped

	if (szSrc == NULL || pnDestLen == NULL)
	{
		return false;
	}
	
	const char * szSrcEnd = szSrc + nSrcLen;
	int nWritten = 0;
	
	bool bOverflow = (pbDest == NULL) ? true : false;
	
	while (szSrc < szSrcEnd &&(*szSrc) != 0)
	{
		unsigned dwCurr = 0;
		int i;
		int nBits = 0;
		for (i=0; i<4; i++)
		{
			if (szSrc >= szSrcEnd)
				break;
			int nCh = DecodeBase64Char(*szSrc);
			szSrc++;
			if (nCh == -1)
			{
				// skip this char
				i--;
				continue;
			}
			dwCurr <<= 6;
			dwCurr |= nCh;
			nBits += 6;
		}

		if(!bOverflow && nWritten + (nBits/8) > (*pnDestLen))
			bOverflow = true;

		// dwCurr has the 3 bytes to write to the output buffer
		// left to right
		dwCurr <<= 24-nBits;
		for (i=0; i<nBits/8; i++)
		{
			if(!bOverflow)
			{
				*pbDest = (unsigned char) ((dwCurr & 0x00ff0000) >> 16);
				pbDest++;
			}
			dwCurr <<= 8;
			nWritten++;
		}

	}
	
	*pnDestLen = nWritten;
	
	if(bOverflow)
	{
		return false;
	}
	
	return true;
}

#endif
