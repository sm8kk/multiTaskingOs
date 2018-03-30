int strcmp(char *a, char *b, int c);
void main()
{
    int i,j,k,z;
    char line[80], buffer[512];
    char param[3][20];
    enableInterrupts();
    while(1)
    {
	for(i=0;i<4;i++)
	    for(j=0;j<20;j++)
		param[i][j]=0;

	for(i=0; i<80; i++)
	    line[i]=0;

	for(i=0;i<512;i++)
	    buffer[i]=0;

        interrupt(0x21,0,"shell#",0,0);
        interrupt(0x21,1,line,0,0);

        // parse the params	
	if(line[0] == 0x0d)
	    continue;
	for(i=0, j=0, k=0; line[i] !='\0'; i++)
	{
	    if(line[i] == ' ')
	    {
		param[j][k] = '\0';
	        j++;
		k = 0;
	    }

	    while(line[i] == ' ')
		i++;

	    param[j][k] = line[i];
	    k++;
	}
  	param[j][k] ='\0';

	//print the parameters entered
	/*
	for(i=0; i<=j; i++)
	{
	    if(i>0)
	    interrupt(0x21,0,":",0,0);
	    interrupt(0x21,0,param[i],0,0);
	}
	*/
	if(strcmp("dir", param[0], 3))
	{
	    interrupt(0x21, 3, 0,0,0);
	}
	else if(strcmp("type", param[0], 4))
	{
	    for(i=0;i<512;i++)
		buffer[i]=0;
	    interrupt(0x21, 6, param[1], buffer, 0);
    	    interrupt(0x21, 0, buffer, 0, 0);
    	    interrupt(0x10, 0xe*256+0x0A, 0, 0, 0);
            interrupt(0x10, 0xe*256+0x0D, 0, 0, 0);
	}
	else if(strcmp("del", param[0], 3))
	{
	    interrupt(0x21, 4, param[1], 0, 0);
	}
	else if(strcmp("create", param[0], 6))
	{
            interrupt(0x21,1,buffer,0,0); //read a line
            interrupt(0x21, 8, param[1], buffer, 0); //write in sector
	}
	else if(strcmp("execute", param[0], 7))
	{
	    interrupt(0x21, 9, param[1], 0x3000, 0);;
	}       
	else if(strcmp("kill", param[0], 7))
	{	
	    interrupt(0x21, 10, param[1][0] - 48, 0, 0);
	}       
	else
	{
	    interrupt(0x21,0,"Bad Command!",0,0);
	}
    }
}

int strcmp(char *a, char *b, int c)
{
    int i,flag;
    flag = 1;
    for(i=0; i<c; i++)
    {
	if(a[i] != b[i])
	{
	    flag = 0;
	    break;
	}
    }
    if(flag == 1)
    return flag;
}
