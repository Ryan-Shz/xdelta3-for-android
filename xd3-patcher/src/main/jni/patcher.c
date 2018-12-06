//
// Permission to distribute this example by
// Copyright (C) 2007 Ralf Junker
// Ralf Junker <delphi@yunqa.de>
// http://www.yunqa.de/delphi/

//---------------------------------------------------------------------------

#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <sys/stat.h>
#include "xdelta3.h"
#include "xdelta3.c"

//---------------------------------------------------------------------------

static int code (
  int encode,
  FILE*  InFile,
  FILE*  SrcFile ,
  FILE* OutFile,
  int BufSize )
{
  int r = -1, ret = -1;
  struct stat statbuf;
  xd3_stream stream;
  xd3_config config;
  xd3_source source;
  void* Input_Buf = 0;
  int Input_Buf_Read;
  

  if (BufSize < XD3_ALLOCSIZE)
    BufSize = XD3_ALLOCSIZE;

  memset (&stream, 0, sizeof (stream));
  memset (&source, 0, sizeof (source));

  xd3_init_config(&config, XD3_ADLER32);
  config.winsize = BufSize;
  xd3_config_stream(&stream, &config);

  if (SrcFile)
  {
    r = fstat(fileno(SrcFile), &statbuf);
    if (r)
    {
      ret = 20;
      goto _fail;
    }

    source.blksize = BufSize;
    source.curblk = malloc(source.blksize);
    if (source.curblk == 0) 
    {
    	ret = 21;
    	goto _fail;
    }

    /* Load 1st block of stream. */
    r = fseek(SrcFile, 0, SEEK_SET);
    if (r)
    {
    	ret = 22;
    	goto _fail;
    }    	

    source.onblk = fread((void*)source.curblk, 1, source.blksize, SrcFile);
    source.curblkno = 0;
    /* Set the stream. */
    xd3_set_source(&stream, &source);
  }

  Input_Buf = malloc(BufSize);
  if (Input_Buf == 0) 
  {
  	ret = 23;
  	goto _fail;
  }

  r = fseek(InFile, 0, SEEK_SET);
  if (r) 
  {
  	ret = 24;
  	goto _fail;
  }
  
  do
  {
    Input_Buf_Read = fread(Input_Buf, 1, BufSize, InFile);
    if (Input_Buf_Read < BufSize)
    {
      xd3_set_flags(&stream, XD3_FLUSH | stream.flags);
    }
    xd3_avail_input(&stream, Input_Buf, Input_Buf_Read);

process:
    if (encode)
      ret = xd3_encode_input(&stream);
    else
      ret = xd3_decode_input(&stream);

    switch (ret)
    {
    case XD3_INPUT:
      {
        continue;
      }

    case XD3_OUTPUT:
      {
        r = fwrite(stream.next_out, 1, stream.avail_out, OutFile);
        if (r != (int)stream.avail_out)
        {
        	ret = 25;
        	goto _fail;
        }

	xd3_consume_output(&stream);
        goto process;
      }

    case XD3_GETSRCBLK:
      {
        if (SrcFile)
        {
          r = fseek(SrcFile, source.blksize * source.getblkno, SEEK_SET);
          if (r)
          {
          	ret = 26;
          	goto _fail;
          }

          source.onblk = fread((void*)source.curblk, 1,
			       source.blksize, SrcFile);
          source.curblkno = source.getblkno;
        }
        goto process;
      }

    case XD3_GOTHEADER:
      {
        goto process;
      }

    case XD3_WINSTART:
      {
        goto process;
      }

    case XD3_WINFINISH:
      {
        goto process;
      }

    default:
      {
        ret = 27;
        goto _fail;
      }

    }

  }
  while (Input_Buf_Read == BufSize);
  
  ret = 0;
    
  _fail:
    if (source.curblk)
    {
        free((void*)source.curblk);
    }
    if (Input_Buf)
    {
        free(Input_Buf);
    }
    
    xd3_close_stream(&stream);
    xd3_free_stream(&stream);
        
    return ret;
}

JNIEXPORT jint  Java_com_ryan_github_patcher_Patcher_applyPatch(
        JNIEnv* env,
        jobject thiz,
        jint encode,
        jstring inPath,
        jstring srcPath ,
        jstring outPath)
{
    const char* inUTFPath;
    const char* srcUTFPath;
    const char* outUTFPath;
    FILE* InFile;
    FILE* SrcFile;
    FILE* OutFile;
    int res=-1;

    inUTFPath = (*env)->GetStringUTFChars(env, inPath, NULL);
    InFile = fopen(inUTFPath,"rb");
    if (InFile <= 0) { 
    	res = 10;
    	goto out3;
    }
    
    (*env)->ReleaseStringUTFChars(env, inPath, inUTFPath);
    srcUTFPath = (*env)->GetStringUTFChars(env, srcPath, NULL);
    SrcFile = fopen(srcUTFPath,"rb");
    if (SrcFile <= 0) {
    	 res = 11;
    	 goto out2;
    }
    
    (*env)->ReleaseStringUTFChars(env, srcPath, srcUTFPath);
    outUTFPath = (*env)->GetStringUTFChars(env, outPath, NULL);
    OutFile = fopen(outUTFPath,"wb");
    if (OutFile <= 0) {
    	res = 12;
    	goto out1;
    }
    
    (*env)->ReleaseStringUTFChars(env, outPath, outUTFPath);
    res = code (encode, InFile, SrcFile, OutFile, 0x1000);

    fclose(OutFile);
out1:
    fclose(SrcFile);
out2:
    fclose(InFile);
out3:
    return res;
}

