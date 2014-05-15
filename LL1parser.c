#include <stdio.h>
#include <string.h>

//finds the array index of the given symbol in the given array
int get_symbol_index(char symbol_array[], char symbol);

//get the first non terminal from the given array
int get_first_nt(char str_used[], int *, char *);

//update the pointer to the next symbol to be derived in the string to be parsed
int update_str_pointer(char str_to_pass[], char sentinal_string[], int *str_pointer);

//get terminals and non-terminals
int getTermNTerm(int no_rules, char rules[no_rules][100], char *non_term_vector, char *term_vector, int *no_nt, int *no_t);

//find first
int find_first(int no_nt,int no_t,int no_rules,char non_term[no_rules],char term[no_t],char rules[no_rules][100],char first[no_nt][no_t+1]);

//find follow
int find_follow(int no_nt,int no_t,int no_rules,char non_term[no_rules],char term[no_t],char rules[no_rules][100],char first[no_nt][no_t+1],char follow[no_nt][no_t+1]);

//make ll1 parsing table
void make_table(int no_nt,int no_t,int no_rules,char non_term[no_rules],char term[no_t],char rules[no_rules][100],char first[no_nt][no_t+1],char follow[no_nt][no_t+1], int parser[no_nt][no_t+1]);

int main(int argc, char *argv[])
{
	int no_nt=0, no_t=100, no_rules=0;
	char str_to_pass[100];
	char sentinal_string[10*strlen(str_to_pass)], temp[10*strlen(str_to_pass)];
	char *rule_prod, *rule_rhs;
	char start_nt, current_nt, current_t;
	int current_nt_index = 0, current_t_index = 0;
	int str_pointer = 0, sentinal_pointer = 0;
	int current_rule_no;
	int value_get_nt, value_update_s;
	char numRarray[100];
	FILE *fp, *fp1;	
	int ruleCount = 0, temp1 = 0, temp2 = 0;
	int checkTnT = 0;
	int code;

	if (argc!=2)
	{
		printf("Improper number of arguments\n");
		return -1;
	}

	fp = fopen(argv[1],"r");

	if (fp == NULL)
	{
		puts("Could not open the file");
		return -1;
	}
	
	numRarray[ruleCount] = getc(fp);
	while (numRarray[ruleCount] != 10)
	{
		temp1 = numRarray[ruleCount] - '0';
		no_rules = no_rules*10 + temp1;
		ruleCount += 1;
		numRarray[ruleCount] = getc(fp);			
	}

	printf("Number of rules: %d \n\n",no_rules);
	char rules[no_rules][100];
	char non_term[no_rules];
	char term[no_t];

	for  (ruleCount=0;ruleCount<no_rules;ruleCount++)
	{
		fgets(rules[ruleCount],100,fp);
			//Because fgets() adds a newline at the end of the string
		if (ruleCount==0)
		{
			//rules[ruleCount][strlen(rules[ruleCount])-1] = '$';
			rules[ruleCount][strlen(rules[ruleCount])-1] = '\0';
		}
		else
		{
			rules[ruleCount][strlen(rules[ruleCount])-1] = '\0';
		}
		
	}

		//get the file path for the string in str_to_pass
	fgets(str_to_pass,100,fp);

	fclose(fp);
	
	getTermNTerm(no_rules,rules,&(non_term[0]),&(term[0]),&no_nt,&no_t);

	printf("\n");

	char first[no_nt][no_t+1];
	char follow[no_nt][no_t+1];
	for (temp1 = 0;temp1<no_nt;temp1++)
	{
		first[temp1][0] = '\0';		
		follow[temp1][0] = '\0';
	}

	int found_first = 0, found_follow = 0;

	found_first = find_first(no_nt,no_t,no_rules,non_term,term,rules,first);

	found_follow = find_follow(no_nt,no_t,no_rules,non_term,term,rules,first,follow);

	int	parser[no_nt][no_t+1];
	int	temp_t;
	for (temp1 = 0;temp1<no_nt;temp1++)
	{
		for (temp_t = 0;temp_t<=no_nt;temp_t++)
		{
			parser[temp1][temp_t] = -1;
		}
	}

	make_table(no_nt,no_t,no_rules,non_term,term,rules,first,follow,parser);
	
	for  (ruleCount=0;ruleCount<no_rules;ruleCount++)
	{
		if (rules[ruleCount][strlen(rules[ruleCount])-1] == 101)
		{
			rules[ruleCount][strlen(rules[ruleCount])-1] = '\0';			
		}
	}
	
	str_to_pass[strlen(str_to_pass)-1] = '\0';
	fp1 = fopen(str_to_pass,"r");

	if (fp1 == NULL)
	{
		puts("Could not open the string file");
		return -1;
	}

	fgets(str_to_pass,100,fp1);

	fclose(fp1);
	
	str_to_pass[strlen(str_to_pass)-1] = '\0';
	int	valid = 0;
	while (valid != 1)
	{	
		valid = 1;
		int i = 0;
		for (i;i<strlen(str_to_pass);i++)
		{
			code = (int)str_to_pass[i];
			if ((code >= 65) && (code <= 90))
			{
				printf("Only terminals are allowed in the string.\nTerminal symbols cannot be capital letters.\n");
				valid = 0;
			}
			else if (code == 101)
			{
				printf("Sorry 'e' is reserved. It cannot be used as a terminal symbol.\n");
				valid = 0;
			}
		}
	}
	
	char end[] = "$";
	strcat(str_to_pass,end);

	printf("\nString Received: %s\n\n",str_to_pass);
	start_nt = non_term[0];
	current_nt = start_nt;	
	sentinal_string[0] = current_nt;
	sentinal_string[1] = '\0';
	temp[0] = '\0';
	while (strcmp(str_to_pass,sentinal_string)!=0)
	{		
		current_nt_index = get_symbol_index(non_term,current_nt);
		if (current_nt_index == -1)
		{
			printf("Undefined Non Terminal found");
			return -1;
		}

		current_t = str_to_pass[str_pointer];
		current_t_index = get_symbol_index(term, current_t);
		current_rule_no = parser[current_nt_index][current_t_index];
		rule_prod = &(rules[current_rule_no][0]);
		rule_rhs = &(rule_prod[2]);
		int index_temp = 0;
		temp[0] = '\0';
		for (index_temp = 0;index_temp < sentinal_pointer; index_temp++)
		{
			temp[index_temp] = sentinal_string[index_temp];
		}
		for (index_temp = 0;index_temp < strlen(rule_rhs); index_temp++)
		{
			{
				temp[sentinal_pointer+index_temp] = rule_rhs[index_temp];
			}
		}
		temp[sentinal_pointer+strlen(rule_rhs)] = '\0';
		strcat(temp,&sentinal_string[sentinal_pointer+1]);
		temp[strlen(temp)] = '\0';
		strcpy(sentinal_string,temp);
		printf("%s\n",sentinal_string);
		
		value_get_nt = get_first_nt(sentinal_string, &sentinal_pointer, &current_nt);
		value_update_s = update_str_pointer(str_to_pass,sentinal_string,&str_pointer);

		if ((value_get_nt == -1)&&(value_update_s == -1))
		{
			printf("String successfully parsed.\n");
			return 1;
		}
	}	
}

int get_symbol_index(char symbol_array[], char symbol)
{
	int symbol_index = 0, symbol_array_index;
	for(symbol_index;symbol_index<strlen(symbol_array);symbol_index++)
	{
		if (symbol_array[symbol_index] == symbol)
		{
			symbol_array_index = symbol_index;
			return (symbol_array_index);
		}
	}
	return -1;
}

int get_first_nt(char str_used[], int *index, char *nt)
{	
	int i = 0;
	for (i;i<strlen(str_used);i++)
	{
		int code = (int)str_used[i];
		if ((code >= 65) && (code <= 90))
		{
			*index = i;
			*nt = str_used[i];
			return 0;
		}
	}	
	return -1;
}

int update_str_pointer(char str_to_pass[], char sentinal_string[], int *str_pointer)
{
	int min_length;
	if (strlen(str_to_pass)<=strlen(sentinal_string))
		min_length = strlen(str_to_pass);
	else 
		min_length = strlen(sentinal_string);	
	
	if (strncmp(str_to_pass,sentinal_string,strlen(sentinal_string)) == 0)
	{
		printf("The string has been parsed successfully!!\n");
		return -1;		
	}

	int i=1, test=0;
	for (i;i<=min_length;i++)
	{
		if (*str_pointer > 0)
		{
			if (test == 0)
			{
				test = strncmp(str_to_pass,sentinal_string,i);
				if (test != 0)
				{
					if (*str_pointer == i-2)
					{
						*str_pointer = *str_pointer + 1;
						return 0;
					}
				}
		
			}
		}
		else if (*str_pointer == 0)
		{
			if (strncmp(str_to_pass,sentinal_string,1)==0)
			{
				*str_pointer = *str_pointer + 1;
				return 0;
			}
		}	
	}
}

int getTermNTerm(int no_rules, char rules[no_rules][100], char *non_term_vector, char *term_vector, int *no_nt, int *no_t)
{
	int ruleCount = 0;
	int temp = 0, temp1 = 0;
	int newNT = 1, newT = 1;
	int i=0,j=0,k=0,code;
	char *non_term = non_term_vector;
	char *term = term_vector;
	for (ruleCount=0;ruleCount<no_rules;ruleCount++)
	{
		printf("%d. %s\n",ruleCount,(rules[ruleCount]));

		for (i=0;i<strlen(rules[ruleCount]);i++)
		{	
			newNT = 1;
			newT = 1;
			code = rules[ruleCount][i];
		
			if ((code >=65) && (code <=90))
			{				
				for (j=0;j<temp;j++)
				{
					if (non_term[j] == rules[ruleCount][i])
					{
						newNT = 0;
					}
				}
				if (newNT == 1)
				{
					non_term[temp] = rules[ruleCount][i];
					temp += 1;
				}				
			}
		
			if (i>1)
			{
				if (!( ((code >=65) && (code <=90)) || (code == 101) ))
				{
					for (k=0;k<temp1;k++)
					{							
						if (term[k] == rules[ruleCount][i])
						{
							newT = 0;
						}					
					}
					if (newT == 1)
					{
						term[temp1] = rules[ruleCount][i];
						temp1 += 1;
					}
				}
			}
		}		
	}
	non_term[temp] = '\0';
	term[temp1] = '$';	
	temp1 += 1;
	term[temp1] = '\0';
	*no_nt = strlen(non_term);
	*no_t = strlen(term);
	printf("\nNon Terminals: %s, number of non-terminals = %d\n",non_term,*no_nt);
	printf("Terminals: %s, number of terminals = %d\n",term,*no_t);
	return 1;
}

int find_first(int no_nt,int no_t,int no_rules,char non_term[no_rules],char term[no_t],char rules[no_rules][100],char first[no_nt][no_t+1])
{
	int code, changed = 1;
	int temp1 = 0, ruleCount = 0, temp2 = 0;;
	char current_t, current_nt;
	int current_t_index,current_nt_index;
	char *rule_rhs;	

	while(changed == 1)
	{	changed = 0;
		for (ruleCount=0;ruleCount<no_rules;ruleCount++)
		{
			current_nt = rules[ruleCount][0];
			current_nt_index = get_symbol_index(non_term,current_nt);
			rule_rhs = &(rules[ruleCount][2]);
			int prev_exist= 0,does_exist = 0;
			for (temp1=0;temp1<strlen(rule_rhs);temp1++)
			{	

				code = rule_rhs[temp1];

				if ((temp1 == 0)&&(code == 101))
				{
					prev_exist= 0;does_exist = 0;
					for (prev_exist=0;prev_exist<strlen(first[current_nt_index]);prev_exist++)
					{
						if (code == first[current_nt_index][prev_exist])
						{
							does_exist = 1;
						}
					}
					if (does_exist == 0)
					{					
						strcat(first[current_nt_index],"e");
						changed = 1;
					}
					break;
				}
				else if ((temp1 == 0)&&(!((code >=65) && (code <=90))))
				{	
					prev_exist= 0;does_exist = 0;
					for (prev_exist=0;prev_exist<strlen(first[current_nt_index]);prev_exist++)
					{
						if (code == first[current_nt_index][prev_exist])
						{
							does_exist = 1;
						}
					}
					if (does_exist == 0)
					{
						strncat(first[current_nt_index],&(rule_rhs[temp1]),1);
						changed = 1;					
					}
					break;				
				}
				else if ((code >=65) && (code <=90))
				{	
					int new_nt_index = get_symbol_index(non_term,rule_rhs[temp1]);
					int is_nullable = 0;
					for (temp2 = 0;temp2 < strlen(first[new_nt_index]);temp2++)
					{
						if (first[new_nt_index][temp2] != 101)
						{	
							prev_exist= 0;
							does_exist = 0;
							for (prev_exist=0;prev_exist<strlen(first[current_nt_index]);prev_exist++)
							{
								if (first[new_nt_index][temp2] == first[current_nt_index][prev_exist])
								{
									does_exist = 1;
								}
							}
							if (does_exist == 0)
							{
								strncat(first[current_nt_index],&(first[new_nt_index][temp2]),1);
								changed = 1;
							}
						}
						else if(first[new_nt_index][temp2] == 101)
						{
							is_nullable = 1;
						}
					}
					if (is_nullable == 0)
					{
						break;
					}				
				}		
			}			
		}
	}

	for (temp1 = 0;temp1<no_nt;temp1++)
	{
		printf("first[%c] = %s\n",non_term[temp1],first[temp1]);		
	}
}

int find_follow(int no_nt,int no_t,int no_rules,char non_term[no_rules],char term[no_t],char rules[no_rules][100],char first[no_nt][no_t+1],char follow[no_nt][no_t+1])
{	int i,rule_count,rule_length_count;
	char *rule_prod, *rule_rhs, *rest_rhs;
	int changed = 1, is_nullable = 0;
	char current_t, current_nt, first_nt;
	int current_t_index,current_nt_index, first_nt_index, first_nt_temp_index;
	int	found_first_nt = 0, rest_rhs_length_count;
	int code, first_length_count, code_index;
	int does_exist, prev_exist, follow_length_count;
	char left_nt;
	int left_nt_index;
	
	printf("\n");

	follow[0][0] = '$';
	follow[0][1] = '\0';
	while(changed == 1)
	{
		changed = 0;
		for	(rule_count=0;rule_count<no_rules;rule_count++)	
		{
			rule_prod = &(rules[rule_count][0]);
			rule_rhs = &(rule_prod[2]);
			rule_length_count=0;
			while(rule_length_count<strlen(rule_rhs))
			{
				rest_rhs = rule_rhs+rule_length_count;
				found_first_nt = get_first_nt(rest_rhs,&first_nt_temp_index, &first_nt);
				if (found_first_nt == -1)
				{
					break;
				}
				first_nt_index = rule_length_count+first_nt_temp_index;
				current_nt = first_nt;
				current_nt_index = get_symbol_index(non_term,current_nt);

				if (first_nt_index == (strlen(rule_rhs)-1))
				{	
					left_nt = rules[rule_count][0];
					left_nt_index = get_symbol_index(non_term,left_nt);
					for (follow_length_count = 0;follow_length_count<strlen(follow[left_nt_index]);follow_length_count++)
					{
						if (follow[left_nt_index][follow_length_count] != 'e')
						{						
							prev_exist= 0;does_exist = 0;
							for (prev_exist=0;prev_exist<strlen(follow[current_nt_index]);prev_exist++)
							{
								if (follow[left_nt_index][follow_length_count] == follow[current_nt_index][prev_exist])
								{
									does_exist = 1;
								}
							}
							if (does_exist == 0)
							{
								strncat(follow[current_nt_index],&(follow[left_nt_index][follow_length_count]),1);
								changed = 1;
							}
						}
					}
				}

				for(rest_rhs_length_count = first_nt_temp_index+1;rest_rhs_length_count < strlen(rest_rhs);rest_rhs_length_count++)
				{
					code = (int)rest_rhs[rest_rhs_length_count];
					
					if (!((code>=65)&&(code<=90)) && !(code == 101))
					{ 
						prev_exist= 0;does_exist = 0;
						for (prev_exist=0;prev_exist<strlen(follow[current_nt_index]);prev_exist++)
						{
							if (code == follow[current_nt_index][prev_exist])
							{
								does_exist = 1;
							}
						}
						if (does_exist == 0)
						{
							strncat(follow[current_nt_index],&(rest_rhs[rest_rhs_length_count]),1);
							changed = 1;
						}
						break;
					}
					else if ((code>=65)&&(code<=90))
					{	
						code_index = get_symbol_index(non_term,rest_rhs[rest_rhs_length_count]);					
						for (first_length_count = 0;first_length_count<strlen(first[code_index]);first_length_count++)
						{
							if (first[code_index][first_length_count] != 'e')
							{
								prev_exist= 0;does_exist = 0;
								for (prev_exist=0;prev_exist<strlen(follow[current_nt_index]);prev_exist++)
								{
									if (first[code_index][first_length_count] == follow[current_nt_index][prev_exist])
									{
										does_exist = 1;
									}
								}
								if (does_exist == 0)
								{
									strncat(follow[current_nt_index],&(first[code_index][first_length_count]),1);
									changed = 1;
								}
							}
							else if (first[code_index][first_length_count] == 'e')
							{
								is_nullable = 1;
								if (rest_rhs_length_count == (strlen(rule_rhs)-1))
								{	
									left_nt = rules[rule_count][0];
									left_nt_index = get_symbol_index(non_term,left_nt);
									for (follow_length_count = 0;follow_length_count<strlen(follow[left_nt_index]);follow_length_count++)
									{
										if (follow[left_nt_index][follow_length_count] != 'e')
										{						
											prev_exist= 0;does_exist = 0;
											for (prev_exist=0;prev_exist<strlen(follow[current_nt_index]);prev_exist++)
											{
												if (follow[left_nt_index][follow_length_count] == follow[current_nt_index][prev_exist])
												{
													does_exist = 1;
												}
											}
											if (does_exist == 0)
											{
												strncat(follow[current_nt_index],&(follow[left_nt_index][follow_length_count]),1);
												changed = 1;
											}
										}
									}
								}
							}
						}
						if (is_nullable == 0)
						{
							break;
						}
					}					
				}				
				rule_length_count = first_nt_index+1;
				//int input;
				//scanf("%d",&input);				
			}
		}
	}
	for	(i=0;i<no_nt;i++)	
	{
		printf("follow[%c] = %s\n",non_term[i],follow[i]);
	}
}

void make_table(int no_nt,int no_t,int no_rules,char non_term[no_rules],char term[no_t],char rules[no_rules][100],char first[no_nt][no_t+1],char follow[no_nt][no_t+1], int parser[no_nt][no_t+1])
{
	char left_nt;
	int rule_count,rule_length_count, t_count_index;
	char first_t, t_count;
	int first_t_index, code, left_nt_index, first_count;
	char *rule_rhs;
	char first_rule[no_t+1];
	int prev_exist, does_exist, is_nullable;
	int code_index,nt_count;
	
	first_rule[0] = '\0';
	for(rule_count = 0;rule_count<no_rules;rule_count++)
	{
		first_rule[0] = '\0';
		left_nt = rules[rule_count][0];
		left_nt_index = get_symbol_index(non_term,left_nt);
		rule_rhs = &(rules[rule_count][2]);
		for(rule_length_count = 0;rule_length_count<strlen(rule_rhs);rule_length_count++)
		{
			code = (int)rule_rhs[rule_length_count];
			if (code == 101)
			{	//printf("epsilon\n");
				strcpy(first_rule,&(follow[left_nt_index][0]));
				//printf("%s: %s \n",rules[rule_count],first_rule);
				break;
			}
			else if (!((code>=65)&&(code<=90)))
			{	//printf("terminal\n");
				prev_exist= 0;does_exist = 0;
				for (prev_exist=0;prev_exist<strlen(first_rule);prev_exist++)
				{
					if (code == first_rule[prev_exist])
					{
						does_exist = 1;
					}
				}
				if (does_exist == 0)
				{
					strncat(first_rule,&(rule_rhs[rule_length_count]),1);
					//printf("%s: %s \n",rules[rule_count],first_rule);
					break;
				}
			}
			else if ((code>=65) && (code<=90))
			{//	printf("non-terminal\n");
				code_index = get_symbol_index(non_term,rule_rhs[rule_length_count]);
				is_nullable = 0;
				for (first_count=0;first_count<strlen(first[code_index]);first_count++)
				{
					first_t = first[code_index][first_count];
					if (first_t != 'e')
					{
						prev_exist= 0;does_exist = 0;
						for (prev_exist=0;prev_exist<strlen(first_rule);prev_exist++)
						{
							if (first_t == first_rule[prev_exist])
							{
								does_exist = 1;
							}
						}
						if (does_exist == 0)
						{
							strncat(first_rule,&first_t,1);
//							printf("%s: %s \n",rules[rule_count],first_rule);
						}
					}
					else if (first_t == 'e')
					{
						is_nullable = 1;
						if(rule_length_count == strlen(rule_rhs)-1)
						{							
							for (t_count=0;t_count<strlen(follow[left_nt_index]);t_count++)
							{
								
								prev_exist= 0;does_exist = 0;
								for (prev_exist=0;prev_exist<strlen(first_rule);prev_exist++)
								{
									if (first_t == first_rule[prev_exist])
									{
										does_exist = 1;
									}
								}
								if (does_exist == 0)
								{
									strncat(first_rule,&(follow[left_nt_index][t_count]),1);
									//printf("%s: %s \n",rules[rule_count],first_rule);
								}
							}
						}
					}
				}
				if (is_nullable == 0)
				{
					break;
				}
			}
		}
		for (first_count=0;first_count<strlen(first_rule);first_count++)
		{
		//	printf("%s: %s \n",rules[rule_count],first_rule);
			first_t = first_rule[first_count];
			first_t_index = get_symbol_index(term,first_t);
			parser[left_nt_index][first_t_index] = rule_count;
		}
	}
	printf("\n   ");
	for (t_count=0;t_count<no_t;t_count++)
	{
		printf("%c    ",term[t_count]);
	}
	printf("\n");
	for (nt_count=0;nt_count<no_nt;nt_count++)
	{	printf("%c  ",non_term[nt_count]);
		for (t_count=0;t_count<no_t;t_count++)
		{
			if (parser[nt_count][t_count] == -1)
			{
				printf("     ",parser[nt_count][t_count]);	
			}else
			{
				printf("%d    ",parser[nt_count][t_count]);
			}
		}
		printf("\n");
	}
}
