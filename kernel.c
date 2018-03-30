/* Kernel
 * Author : Sourav Maji
 */

void main2();
void main() { main2(); }

// Process table definition
struct PCB
{
    int active;
    int sp;
} pcb[8];

int CurrentProcess;
void killProcess(int);
void handleTimerInterrupt(int segment, int sp);
void terminate();
void integertoString(int a, char *s);
void executeProgram(char* name);
void writeFile(char* filename, char inbuf[]);
void readFile(char* filename, char outbuf[]);
void deleteFile(char* filename);
void directory();
void printString(char *);
void readString(char s[]);
void readSector(char *, int);
void writeSector(char *, int);
int divide(int, int);
int modulo(int, int); //deprecated
int modulo1(int a, int b);
void handleInterrupt21 (int, int, int, int);

void main2()
{
    char line[80], buffer[13312];
    int i;
    // initialise the process control block
    for(i=0; i<8; i++)
    {
	pcb[i].active = 0;
	pcb[i].sp = 0xff00;
    }
    
    CurrentProcess = 0;
/* Block previous test cases
    line[0] = 'S';
    line[1] = 'o';
    line[2] = 'u';
    line[3] = 'r';
    line[4] = 'a';
    line[5] = 'v';
    line[6] = '\0';
    printString(line);
*/
    // Test case for Interrupt21
    makeInterrupt21();
    //interrupt(0x21,0,line,0,0);
    //interrupt(0x21,1,line,0,0);
    
    // Test case for print and read subroutines
/*    printString("Enter a line :");
    readString(line);
    printString(" Line:");
    printString(line);

    // Test case to read a Floppy sector and print the same
    readSector(buffer, 30); 
    printString(buffer);
*/
/*
    // Q1: Directory listing
    interrupt(0x21, 3, 0,0,0);
    // delete file
    interrupt(0x21, 4, "messag", 0, 0);
    interrupt(0x21, 3, 0,0,0);
*/
/*
    // Q2:
    interrupt(0x21, 6, "messag", buffer, 0);
    interrupt(0x21, 0, buffer, 0, 0);
    interrupt(0x10, 0xe*256+0x0A, 0, 0, 0);
    interrupt(0x10, 0xe*256+0x0D, 0, 0, 0);
    interrupt(0x21,1,line,0,0);
    interrupt(0x21,0,line,0,0);
*/

/*
   // Q3 : Read file in buffer
    interrupt(0x21,3,0,0,0);
    interrupt(0x21, 6, "messag", buffer, 0);
    printString(buffer);
    interrupt(0x21, 8, "c_mess", buffer, 0);
    interrupt(0x21,3,0,0,0);	
*/
/*
    // Q4: Execute a Program
    interrupt(0x21, 9, "tstprg", 0x2000, 0);
*/  
    //interrupt(0x21, 9, "tstpr2", 0x2000, 0);
  
    //Q5: Shell
    makeTimerInterrupt();
    interrupt(0x21, 9, "shell", 0, 0);

  while(1);
}

void handleTimerInterrupt(int segment, int sp)
{
	int i,j,index;
	char buf[5];
	// save the current process sp.
	if(segment != 0x1000) 
	{ 
            setKernelDataSegment();
	    pcb[CurrentProcess].sp = sp;
	    i=CurrentProcess + 1;
	    restoreDataSegment();
	
	    //integertoString(i, buf);
	    //printString(buf);
	    for(j=0;j<8; i++, j++)
	    {
	        index = modulo1(i,8);
	        //integertoString(index, buf);
	        //printString(buf);
                setKernelDataSegment();
	        if(pcb[index].active)
	        {
		   if(CurrentProcess == index)
		      break;
        	   //printString("Tic ");
	           //integertoString(index, buf);
	           //printString(buf);
		   sp = pcb[index].sp;
		   segment = 0x2000 + index*0x1000;
		   CurrentProcess = index;
	    	   restoreDataSegment();
		   break;
	        }
	    restoreDataSegment();
	    }	  
	}
        //printString("Tic ");

        if (segment == 0x1000) {
	    putInMemory(0xB000, 0x8162, 'K');
	    putInMemory(0xB000, 0x8163, 0x7);
	   
            setKernelDataSegment();
	    if(pcb[CurrentProcess].active)
	    {
        	//printString("Tac ");
	        //integertoString(CurrentProcess, buf);
	        //printString(buf);
	        sp = pcb[CurrentProcess].sp;
	        segment = 0x2000 + CurrentProcess*0x1000;
	    	restoreDataSegment();
	    }
	}
	else if (segment == 0x2000) {
	putInMemory(0xB000, 0x8164, '0');
	putInMemory(0xB000, 0x8165, 0x7);
	}
	else if (segment == 0x3000) {
	putInMemory(0xB000, 0x8166, '1');
	putInMemory(0xB000, 0x8167, 0x7);
	}
	else {
	putInMemory(0xB000, 0x8160, 'X');
	putInMemory(0xB000, 0x8161, 0x7);
	}

        returnFromTimer(segment, sp);
	
}

int modulo1(int a, int b)
{
	int i;
	if(a < b)
	    return a;
	
	i=a;
        while(i >= b)
	{
	    i = i - b;
	}
	return i;
}
	
void printString(char *s)
{
    int i;
    for(i = 0; *(s+i)!='\0'; i++)
    interrupt(0x10, 0xe*256+s[i], 0, 0, 0);
}

void readString(char s[])
{
    int i, Ax, Bx, Cx, Dx ;
    char y, dh, dl;
    for(i=0; ;i++) {
        y = interrupt(0x16, 0, 0, 0, 0);
        if(y != 0xd) {
            if(y == 0x8 && i > 0) {
                i=i-2; // -2 because i increments for backspace in for loop
	    } else {
                s[i] = y;
                interrupt(0x10, 0xe*256+s[i], 0, 0, 0);
           }		
        } else {
            s[i] = 0x0d;
            interrupt(0x10, 0xe*256+s[i], 0, 0, 0);
	    i=i+1;
            s[i] = 0x0a;
            interrupt(0x10, 0xe*256+s[i], 0, 0, 0);
	    i =i+1;
	    s[i] = '\0';
            break;
        }
    }
}

void readSector(char *buffer, int sector)
{
    char Ah, Al, Ch, Cl, Dh, Dl, i, buf[10];
    int Ax, Bx, Cx, Dx;
    /* USAGE
    AH = 2 (this number tells the BIOS to read a sector as opposed to write)
    AL = number of sectors to read (use 1)
    BX = address where the data should be stored to (pass your char* array here) CH = track number
    CL = relative sector number
    DH = head number
    DL = device number (for the floppy disk, use 0)
    */
    Dl = 0;
    Bx = buffer;
    Ah = 0x2; // read operation
    Al = 1; // One sector to read
    Cl = modulo1(sector, 18) + 1; 
    Dh = modulo1(divide(sector, 18), 2);
    Ch = divide(sector, 36);
    Dx=Dh*256+Dl;
    Cx=Ch*256+Cl;
    Ax=Ah*256+Al;

    interrupt(0x13, Ax, Bx, Cx, Dx);
}

void directory()
{
	char dir[512], list[7];
	int i,j;
	readSector( dir, 2);
 	for(i=0; i < 512; i = i+ 0x20)
	{
	    if(dir[i] != 0)
	    {
	        for(j=0; j<6; j++)
		{
		    list[j]=dir[i+j];
		}
		list[j] = '\0';
		printString(list);
    		interrupt(0x10, 0xe*256+0x0A, 0, 0, 0);
    		interrupt(0x10, 0xe*256+0x0D, 0, 0, 0);
		//printString(" ");
	    }
	}
}
		
void deleteFile(char* filename)
{
	char dir[512], list[7], map[512];
        int i,j,flag;
	readSector( map, 1);
        readSector( dir, 2);
	for(i=0; i < 512; i = i+ 0x20)
        {
            if(dir[i] != 0)
            {	
		flag = 1;
                for(j=0; j<6; j++)
                {
                    if(dir[i+j]!=filename[j])
		    {
		        flag = 0;
			break;
		    }
                }
	    // if flag==1, means there is a hit
	    if(flag)
		break;
	    }		
         }
	if(flag)
	{
	    dir[i] = 0; //make directory entry 0;
	    // now 0 out the sector information in map
	    for(j=6; j<32; j++)
	    {
	        if(dir[i+j]!=0)
	            map[dir[i+j]] = 0;
	    }
	    //write out the map and dir sectors
	    writeSector(map, 1);
	    writeSector(dir, 2);
        }
}

void readFile(char* filename, char outbuf[])
{
	char dir[512], list[7], map[512];
	char sector_buf[512];
        int i,j,flag, sector, index,k;
        readSector( map, 1);
        readSector( dir, 2);
        for(i=0; i < 512; i = i+ 0x20)
        {
            if(dir[i] != 0)
            {
                flag = 1;
                for(j=0; j<6; j++)
                {
                    if(dir[i+j]!=filename[j])
                    {
                        flag = 0;
                        break;
                    }
                }
            // if flag==1, means there is a hit
            if(flag)
                break;
            }
        }
	if(flag == 0)
	    return;
	
	index = 0;
	for(j=6; j<32; j++)
	{
	    if(dir[i+j]!=0)
	    {
		sector = dir[i+j];
	    	if(map[sector]!= 0)
		{
		    readSector(sector_buf,sector);
		    for(k=0; k<512; k++, index++)
		    outbuf[index] = sector_buf[k];
		}
	    }
	}
}
	
void writeFile(char* filename, char inbuf[])
{
	char dir[512], list[7], map[512];
        char sector_buf[512], val[10];
        int i,j,flag, sector,index,k,l,ex, sec_buf_in, z;
        readSector( map, 1);
        readSector( dir, 2);
	
	for(i=0; i<512; i++)
	{
	    if(map[i] == 0)
		break;
	}
	
	//No space to write	
	if(i==512)
	return;
	
	for(j=0; j<512; j=j+0x20)
	{
	    if(dir[j] == 0)
	        break;
	}
	
	if(j==512)
	return;
	
	for(k=0; k<6; k++)
	{
	    dir[j+k] = filename[k];
	}
	index = j+k;
	sector = i;
	map[sector]=0xFF;
	dir[index]=sector;
	k = k+1;
	for(l=0, sec_buf_in = 0; inbuf[l]!='\0';)
	{
	    sector_buf[sec_buf_in] = inbuf[l];
	    l = l+1;
	    sec_buf_in ++;
	    ex = modulo1(l,512);
	    // Without this only one sector can be written
	    if(modulo1(l,512) == 0)
	    {
		writeSector(sector_buf,sector); 
		sec_buf_in = 0;
		for(i=0; i<512; i++)
		{
		    if(map[i] == 0)
			break;
		}
		sector = i;
        	index ++;
		k++;
		map[sector]=0xFF;
		dir[index]=sector;
		if(k > 31)
		break;
	    }
	    	 
	}
	writeSector(map, 1);
	writeSector(dir, 2);

	// if a string is not null terminated it does that
	for(z=0;sector_buf[z]!=0xa;z++);
	
	for(;z<512;z++)
	sector_buf[z]=0;

	writeSector(sector_buf, sector);
}

void executeProgram(char* name)
{
	char buffer[4096], buf[10];
	int i, segment, active, j;

	//Determine a segment
	readFile(name, buffer);
	for(i=0; i<8; i++)
	{
	    setKernelDataSegment();
	    active = pcb[i].active;
	    restoreDataSegment();
	    if(active == 0)
	    {
		segment = 0x2000 + i*0x1000;
		j = i;
		break;
	    }
	}

	for(i=0; i<4096; i++)
	putInMemory(segment, i , buffer[i]);

        setKernelDataSegment();
	//CurrentProcess = j;
	pcb[j].active = 1;
		 //printString("Toe");
	         //integertoString(CurrentProcess, buf);
	         //printString(buf);
	restoreDataSegment();

	//launchProgram(segment);
	initializeProgram(segment);
}

void writeSector(char *buffer, int sector)
{
    char Ah, Al, Ch, Cl, Dh, Dl, i, buf[10];
    int Ax, Bx, Cx, Dx;
    /* USAGE
    AH = 03 (this number tells the BIOS to write to a sector)
    AL = number of sectors to write (use 1)
    BX = address where the data is stored(pass your char* array here) CH = track number
    CL = relative sector number
    DH = head number
    DL = device number (for the floppy disk, use 0)
    */
    Bx = buffer;
    Ah = 0x3; // write operation
    Al = 1; // One sector to write
    Dl = 0;
    Cl = modulo1(sector, 18) + 1; 
    Dh = modulo1(divide(sector, 18), 2);
    Ch = divide(sector, 36);

    Dx=Dh*256+Dl;
    Cx=Ch*256+Cl;
    Ax=Ah*256+Al;
    interrupt(0x13, Ax, Bx, Cx, Dx);
}

int divide(int numerator, int denominator)
{
    int i = 0;
    for(i = 0; numerator >= denominator; i++)
	numerator = numerator - denominator;
    
    return i;
}

int modulo(int a, int b)
{
    int rem;
    rem = a - b*(divide(a,b));
    return rem;
}

void integertoString(int x, char *s)
{
	int i,j;
	char a[10];
        if( x == 0)
	{
	    s[0]='0';
	    s[1]='\0';
	    return;
	}
	for(i=x, j=0; i > 0; i=divide(i,10), j++)
	    a[j] = modulo1(i,10) + 48;
	    a[j] = '\0';
	   
	for(j=j-1, i=0; j>=0; j--, i++)
	    s[i]=a[j];

	s[i]='\0';
}

void terminate()
{
    //use putinmemory
    setKernelDataSegment();
    pcb[ CurrentProcess ].active = 0;
    pcb[ CurrentProcess ].active = 0xff00;
    restoreDataSegment();
    while (1);
}
	
void killProcess(int x)
{
    //use putinmemory
    setKernelDataSegment();
    pcb[x].active = 0;
    pcb[x].active = 0xff00;
    restoreDataSegment();
    //while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX)
{
    switch (AX) {
	case 0: 
            printString(BX);
            break;
        case 1:
            readString(BX);
            break;
        case 2:
            readSector(BX,CX);
            break;
        case 3:
            directory();
            break;
        case 4:
            deleteFile(BX);
            break;
        case 5:
            terminate();
            break;
        case 6:
            readFile(BX,CX);
            break;
        case 7:
            writeSector(BX,CX);
            break;
        case 8:
            writeFile(BX,CX);
            break;
        case 9:
            executeProgram(BX);
            break;
        case 10:
            killProcess(BX);
            break;
        default:
            //printString("Error");
            return;
    }
}
