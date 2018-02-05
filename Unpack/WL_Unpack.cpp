/* ******************************************************************** **
** @@ WL Unpack
** @  Copyrt : 
** @  Author : 
** @  Update :
** @  Update :
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.riData=.text /MERGE:.iData=.text /SECTION:.text,EWR /IGNORE:4078")
#endif 

#include "..\shared\file_find.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

// Shared with WL Pack paired utility. Should be some!
static char pMap[] = "!#$%&'()*+,./0123456789:;<=>?@[\\]^_`{|}~";

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ IsMapPrefix()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static bool IsMapPrefix(int iChr)
{
   int   iLen = sizeof(pMap) - 1;

   for (int ii = 0; ii < iLen; ++ii)
   {
      if (((BYTE)pMap[ii]) == ((BYTE)iChr))
      {
         return true;
      }
   }

   return false;
}

/* ******************************************************************** **
** @@ FindMatch()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static int FindMatch(int iMatch)
{
   int   iLen = sizeof(pMap) - 1;

   for (int ii = 0; ii < iLen; ++ii)
   {
      if (((BYTE)pMap[ii]) == ((BYTE)iMatch))
      {
         return ii;
      }
   }

   // Error !
   ASSERT(0);   

   return -1;
}

/* ******************************************************************** **
** @@ Proceed()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void Proceed(const char* const pszFileName)
{
   char     pszBackName[_MAX_PATH];
   char     pszDrive   [_MAX_DRIVE];
   char     pszDir     [_MAX_DIR];
   char     pszFName   [_MAX_FNAME];
   char     pszExt     [_MAX_EXT];

   _splitpath(pszFileName,pszDrive,pszDir,pszFName,pszExt);
   _makepath( pszBackName,pszDrive,pszDir,pszFName,"org");

   CopyFile(pszFileName,pszBackName,FALSE);

   FILE*    pIn = fopen(pszBackName,"rt");

   if (!pIn)
   {
      perror("\a\nOpen Input File Error !\n");
      return;
   }

   FILE*    pOut = fopen(pszFileName,"wt");
   
   if (!pOut)
   {
      perror("\a\nOpen Output File Error !\n");
      fclose(pIn);
      pIn = NULL;
      return;
   }

   char     pPrev[MAX_PATH];
   char     pNext[MAX_PATH];
   char     pPack[MAX_PATH];

   memset(pPrev,0,MAX_PATH);
   memset(pNext,0,MAX_PATH);
   memset(pPack,0,MAX_PATH);

   while (fgets(pNext,MAX_PATH,pIn))
   {
      pNext[MAX_PATH - 1] = 0; // Ensure ASCIIZ !

      DWORD    dwEOL = strcspn(pNext,"\r\n");

      pNext[dwEOL] = 0;  // Remove EOL chars

      int   iLen = strlen(pNext);

      if (!iLen)
      {
         continue;
      }

      int   iMatch = *pNext;

      if (!IsMapPrefix(iMatch))
      {
         // Is not packed
         strcpy(pPrev,pNext);

         fprintf(pOut,"%s\n",pNext);
      }
      else
      {
         int   iCnt = FindMatch(iMatch);

         if (iCnt != -1)
         {
            strncpy(pPack,pPrev,iCnt);
            pPack[iCnt] = 0; // Ensure ASCIIZ

            strcpy(&pPack[iCnt],pNext + 1);
            strcpy(pPrev,pPack);

            fprintf(pOut,"%s\n",pPack);
         }
      }
   }

   fclose(pIn);
   pIn = NULL;

   fclose(pOut);
   pOut = NULL;
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void ShowHelp()
{
   const char  pszCopyright[] = "-*-   WL Unpack 1.0   *   Copyright (c) Gazlan 2014   -*-";
   const char  pszDescript [] = "Wordlist unpacker";
   const char  pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: wl_unpack.com Dix.pak\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int main(int argc,char** argv)
{
   // initialize MFC and print and error on failure
   if (!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(),0))
   {
      return -1;
   }

   if (argc < 2)
   {
      ShowHelp();
      return 0;
   }

   if (argc == 2)
   {
      if ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h")))
      {
         ShowHelp();
      }
   }

   FindFile   FF;

   FF.SetMask(argv[1]);

   while (FF.Fetch())
   {  
      if ((FF._w32fd.dwFileAttributes | FILE_ATTRIBUTE_NORMAL) && !(FF._w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
         Proceed(FF._w32fd.cFileName);
      }
   }
   
   return 0;
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
