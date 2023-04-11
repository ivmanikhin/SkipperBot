#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void get_substring(char* string, char* substr, int pos, int length)
{
    int i = 0;
    while (i < length)
    {
        substr[i] = string[pos+i];
        i++;
    }
    substr[i] = '\0';
}



char** split_by_2_separators(char* input, char* separator_1, char* separator_2, int sent_num, int max_sen_len)
{
    int sentence = 0;
    if (max_sen_len == 0)
	{
		max_sen_len = 128;
	}
    if (sent_num == 0)
    {
    	sent_num = 16;
    }
    int sep_1_len = strlen(separator_1);
    int sep_2_len = strlen(separator_2);
    char temp_substr_1[sep_1_len];
    char temp_substr_2[sep_2_len];
    char cur_char = '1';
    int i = 0;
    char* temp_sentence = (char*)malloc(sizeof(char) * (max_sen_len+1));
    strcpy(temp_sentence, "");
    char** sentences = (char**)malloc(sizeof(char*) * (sent_num));
    for (int i = 0; i < sent_num; i++)
    {
        sentences[i] = (char*)malloc(sizeof(char) * (max_sen_len+1));
        strcpy(sentences[i], "");
    }
    int sentence_i = 0;
    int j = 0;
    cur_char = '1';
    i = 0;
    while (cur_char!='\0' && sentence_i < sent_num)
    {
        get_substring(input, temp_substr_1, i, sep_1_len);
        get_substring(input, temp_substr_2, i, sep_2_len);
        // printf("Temp substr 1: %s     ", temp_substr_1);
        // printf("Temp substr 2: %s\n", temp_substr_2);
        cur_char = *(input+i);
        if (!strcmp(temp_substr_1, separator_1) && sentence == 0)
        {
            sentence = 1;
        }
        else if ((!strcmp(temp_substr_2, separator_2) && sentence == 1))
        {
            sentence = 0;
            temp_sentence[j] = '\0';
            strcpy(sentences[sentence_i], temp_sentence);
            // printf("   Sentence: %s\n", sentences[sentence_i]);
            sentence_i++;
            strcpy(temp_sentence, "");
            j = 0;
        }
        if (sentence == 1)
        {
            temp_sentence[j] = cur_char;
            temp_sentence[j+1] = '\0';
            j++;
        }
        i++;
    }
    return sentences;
}




char** split_to_sentences(char* input, char* separator, int sent_num, int max_sen_len)
{
//    if (input == NULL)
//    {
//        return NULL;
//    }
    if (max_sen_len == 0)
	{
		max_sen_len = 128;
	}
    if (sent_num == 0)
    {
    	sent_num = 16;
    }
//    int inp_len = strlen(input);
    int sep_len = strlen(separator);
    char temp_substr[sep_len];
    char cur_char = '1';
    int i = 0;

    char* temp_sentence = (char*)malloc(sizeof(char) * (max_sen_len + 1));
    strcpy(temp_sentence, "");
    char** sentences = (char**)malloc(sizeof(char*) * (sent_num));
    for (int i = 0; i < sent_num; i++)
    {
        sentences[i] = (char*)malloc(sizeof(char) * (max_sen_len + 1));
        strcpy(sentences[i], "");
    }
    int sentence_i = 0;
    int j = 0;
    cur_char = '1';
    i = 0;
    while (cur_char!='\0' && sentence_i < sent_num )
    {
        get_substring(input, temp_substr, i+1, sep_len);
        cur_char = *(input+i);
        temp_sentence[j] = cur_char;
        temp_sentence[j+1] = '\0';
        j++;
        if (strlen(temp_sentence)>0)
        {
            if (!strcmp(temp_substr, separator) || cur_char == '\0')
            {
                temp_sentence[j] = '\0';
                i--;
                strcpy(sentences[sentence_i], temp_sentence);
                sentence_i++;
                strcpy(temp_sentence, "");
                j = -1;
            }
        }
        i++;
    }
    return sentences;
}
