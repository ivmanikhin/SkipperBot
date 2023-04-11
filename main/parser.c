#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;
}


void get_substring(const char* string, char* substr, const int pos, const int length)
{
    int i = 0;
    while (i < length)
    {
        substr[i] = string[pos+i];
        i++;
    }
    substr[i] = '\0';
}



char** split_by_2_separators(const char* input, const char* separator_1, const char* separator_2, int sent_num, int max_sen_len)
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
    const int sep_1_len = strlen(separator_1);
    const int sep_2_len = strlen(separator_2);
    char temp_substr_1[sep_1_len];
    char temp_substr_2[sep_2_len];
    char cur_char = '1';
    int i = 0;
    char* temp_sentence = (char*)malloc(sizeof(char) * (max_sen_len+1));
    // char sentences[sent_num][max_sen_len];
    char** sentences = (char**)malloc(sizeof(char*) * sent_num);
    for (int i = 0; i < sent_num; i++)
    {
        sentences[i] = (char*)malloc(sizeof(char) * (max_sen_len+1));
    }
    int sentence_i = 0;
    int j = 0;
    while (cur_char!='\0' && sentence_i < sent_num)
    {
        get_substring(input, temp_substr_1, i, sep_1_len);
        get_substring(input, temp_substr_2, i, sep_2_len);
        cur_char = *(input+i);
        if (!strcmp(temp_substr_1, separator_1) && sentence == 0)
        {
            sentence = 1;
        }
        else if ((!strcmp(temp_substr_2, separator_2) && sentence == 1))
        {
            sentence = 0;
            temp_sentence[j] = '\0';
            printf("    Temp sentence: %s\n", temp_sentence);
            strncpy (sentences[sentence_i], temp_sentence, strlen(temp_sentence));
            sentence_i++;
            temp_sentence[0] = '\0';
            j = 0;
        }
        if (sentence == 1)
        {
            printf("   j: %i   cur_char: %c    Sent status: %i\n", j, cur_char, sentence);
            temp_sentence[j] = cur_char;
            temp_sentence[j+1] = '\0';
            j++;
        }
        i++;
    }
    return sentences;
}




char** split_to_sentences(const char* input, const char* separator, int sent_num, int max_sen_len)
{
    if (max_sen_len == 0)
	{
		max_sen_len = 128;
	}
    if (sent_num == 0)
    {
    	sent_num = 16;
    }
//    int inp_len = strlen(input);
    const int sep_len = strlen(separator);
    char temp_substr[sep_len];
    char cur_char = '1';
    int i = 0;
    char* temp_sentence = (char*)malloc(sizeof(char) * (max_sen_len + 1));
    // strcpy(temp_sentence, "");
    char** sentences = (char**)malloc(sizeof(char*) * (sent_num));
    for (int i = 0; i < sent_num; i++)
    {
        sentences[i] = (char*)malloc(sizeof(char) * (max_sen_len + 1));
        // strcpy(sentences[i], "");
    }
    int sentence_i = 0;
    int j = 0;
    cur_char = '1';
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
                memmove(temp_sentence, temp_sentence+sep_len, strlen(temp_sentence));
                strncpy(sentences[sentence_i], temp_sentence, strlen(temp_sentence));
                sentence_i++;
                strcpy(temp_sentence, "");
                // temp_sentence[0] = '\0';
                j = -1;
            }
        }
        i++;
    }
    return sentences;
}
