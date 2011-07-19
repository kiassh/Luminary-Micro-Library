//*****************************************************************************
//
// lmi_fs.c - File System Processing for lwIP Web Server Apps.
//
// Copyright (c) 2009-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 7611 of the EK-LM3S9B92 Firmware Package.
//
//*****************************************************************************

#include <string.h>
#include "utils/lwiplib.h"
#include "httpserver_raw/httpd.h"
#include "httpserver_raw/fs.h"
#include "httpserver_raw/fsdata.h"

//*****************************************************************************
//
// Include the file system data for this application.  This file is generated
// by the makefsfile utility, using the following command:
//
//     makefsfile -i fs -o lmi-fsdata.h -r -h
//
// If any changes are made to the static content of the web pages served by the
// application, this command must be used to regenerate lmi-fsdata.h in order
// for those changes to be picked up by the web server.
//
//*****************************************************************************
#include "lmi-fsdata.h"

//*****************************************************************************
//
// Initialize the file system.
//
//*****************************************************************************
void
fs_init(void)
{
    //
    // Nothing special to do for this application.  Flash File System only.
    //
}

//*****************************************************************************
//
// File System tick handler.
//
//*****************************************************************************
void
fs_tick(unsigned long ulTickMS)
{
    //
    // Nothing special to do for this application.  Flash File System only.
    //
}

//*****************************************************************************
//
// Open a file and return a handle to the file, if found.  Otherwise,
// return NULL.
//
//*****************************************************************************
struct fs_file *
fs_open(char *name)
{
    const struct fsdata_file *ptTree;
    struct fs_file *ptFile = NULL;

    //
    // Allocate memory for the file system structure.
    //
    ptFile = mem_malloc(sizeof(struct fs_file));
    if(NULL == ptFile)
    {
        return(NULL);
    }

    //
    // Initialize the file system tree pointer to the root of the linked list.
    //
    ptTree = FS_ROOT;

    //
    // Begin processing the linked list, looking for the requested file name.
    //
    while(NULL != ptTree)
    {
        //
        // Compare the requested file "name" to the file name in the
        // current node.
        //
        if(strncmp(name, (char *)ptTree->name, ptTree->len) == 0)
        {
            //
            // Fill in the data pointer and length values from the
            // linked list node.
            //
            ptFile->data = (char *)ptTree->data;
            ptFile->len = ptTree->len;

            //
            // For now, we setup the read index to the end of the file,
            // indicating that all data has been read.
            //
            ptFile->index = ptTree->len;

            //
            // We are not using any file system extensions in this
            // application, so set the pointer to NULL.
            //
            ptFile->pextension = NULL;

            //
            // Exit the loop and return the file system pointer.
            //
            break;
        }

        //
        // If we get here, we did not find the file at this node of the linked
        // list.  Get the next element in the list.
        //
        ptTree = ptTree->next;
    }

    //
    // If we didn't find the file, ptTee will be NULL.  Make sure we
    // return a NULL pointer if this happens.
    //
    if(NULL == ptTree)
    {
        mem_free(ptFile);
        ptFile = NULL;
    }

    //
    // Return the file system pointer.
    //
    return(ptFile);
}

//*****************************************************************************
//
// Close an opened file designated by the handle.
//
//*****************************************************************************
void
fs_close(struct fs_file *file)
{
    //
    // If a Fat file was opened, free its object.
    //
    if(file->pextension)
    {
        mem_free(file->pextension);
    }

    //
    // Free the main file system object.
    //
    mem_free(file);
}

//*****************************************************************************
//
// Read the next chunck of data from the file.  Return the count of data
// that was read.  Return 0 if no data is currently available.  Return
// a -1 if at the end of file.
//
//*****************************************************************************
int
fs_read(struct fs_file *file, char *buffer, int count)
{
    int iAvailable;

    //
    // Check to see if more data is available.
    //
    if(file->len == file->index)
    {
        //
        // There is no remaining data.  Return a -1 for EOF indication.
        //
        return(-1);
    }

    //
    // Determine how much data we can copy.  The minimum of the 'count'
    // parameter or the available data in the file system buffer.
    //
    iAvailable = file->len - file->index;
    if(iAvailable > count)
    {
        iAvailable = count;
    }

    //
    // Copy the data.
    //
    memcpy(buffer, file->data + iAvailable, iAvailable);
    file->index += iAvailable;

    //
    // Return the count of data that we copied.
    //
    return(iAvailable);
}
