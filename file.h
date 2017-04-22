
#ifndef FILE_H_
#define FILE_H_

#include "emp_type.h"


typedef int FILE;

BOOLEAN file_write( FILE, INT8U );
//void putc1( FILE, char );

INT8U files_init();

BOOLEAN file_read( FILE file, INT8U *pch );


#endif /* FILE_H_ */
