#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <esp_heap_caps.h>


void concat(char* result, const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    //char *result = heap_caps_malloc(len1 + len2 + 1, MALLOC_CAP_8BIT); // +1 for the null-terminator
    // in real code you would check for errors in heap_caps_malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
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



void split_by_2_separators(char** sentences, const char* input, const char* separator_1, const char* separator_2, int sent_num, int max_sen_len)
{
    const int sizeof_char = sizeof(char);
    // const int sizeof_addr = sizeof(char*);
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
    char* temp_substr_1 = (char*)heap_caps_malloc(sizeof_char * (sep_1_len+1), MALLOC_CAP_8BIT);
    char* temp_substr_2 = (char*)heap_caps_malloc(sizeof_char * (sep_2_len+1), MALLOC_CAP_8BIT);
    char cur_char = '1';
    int i = 0;

    //char* temp_sentence = (char*)heap_caps_malloc(sizeof_char * (max_sen_len+1), MALLOC_CAP_8BIT);
    // char sentences[sent_num][max_sen_len];
    // char** sentences = (char**)heap_caps_malloc(sizeof_addr * sent_num, MALLOC_CAP_8BIT);
    // for (int i = 0; i < sent_num; i++)
    // {
    //     sentences[i] = (char*)heap_caps_malloc(sizeof_char * (max_sen_len+1), MALLOC_CAP_8BIT);
    // }
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
            // sentences[sentence_i] = (char*)heap_caps_malloc(sizeof_char * (max_sen_len+1), MALLOC_CAP_8BIT);
            j = 0;
        }
        else if ((!strcmp(temp_substr_2, separator_2) && sentence == 1))
        {
            sentence = 0;
            sentences[sentence_i][j] = '\0';
            // printf("    Temp sentence: %s\n    Sent num %i\n    Sent %i\n", temp_sentence, sent_num, sentence_i);
            //printf("    Temp sentence: %s\n    Temp sen len: %i\n    Sent num %i\n    Sent %i size %i\n", temp_sentence, strlen(temp_sentence), sent_num, sentence_i, sizeof(sentences[sentence_i]));
            //strcpy (sentences[sentence_i], temp_sentence);
            sentence_i++;
            // temp_sentence[0] = '\0';
        }
        if (sentence == 1)
        {
            sentences[sentence_i][j] = cur_char;
            // temp_sentence[j+1] = '\0';
            j++;
        }
        i++;
    }
    free(temp_substr_1);
    free(temp_substr_2);
    //free(temp_sentence);

}




void split_to_sentences(char** sentences, const char* input, const char* separator, int sent_num, int max_sen_len)
{
    if (max_sen_len == 0)
	{
		max_sen_len = 128;
	}
    if (sent_num == 0)
    {
    	sent_num = 16;
    }
    const int sizeof_char = sizeof(char);
    //const int sizeof_addr = sizeof(char*);
//    int inp_len = strlen(input);
    const int sep_len = strlen(separator);

    char* temp_substr = (char*)heap_caps_malloc(sizeof_char * (sep_len + 1), MALLOC_CAP_8BIT);
    char cur_char = '1';
    int i = 0;
    //char* temp_sentence = (char*)heap_caps_malloc(sizeof_char * (max_sen_len + 1), MALLOC_CAP_8BIT);
    // strcpy(temp_sentence, "");
 //   char** sentences = (char**)heap_caps_malloc(sizeof_addr * (sent_num), MALLOC_CAP_8BIT);
    // for (int i = 0; i < sent_num; i++)
    // {
    //     sentences[i] = (char*)heap_caps_malloc(sizeof_char * (max_sen_len + 1), MALLOC_CAP_8BIT);
    //     // strcpy(sentences[i], "");
    // }
    int sentence_i = 0;
    int j = 0;
    cur_char = '1';
    while (cur_char!='\0' && sentence_i < sent_num )
    {
        get_substring(input, temp_substr, i+1, sep_len);
        cur_char = *(input+i);
        sentences[sentence_i][j] = cur_char;
        j++;
        if (strlen(sentences[sentence_i])>0)
        {
            if (!strcmp(temp_substr, separator) || cur_char == '\0')
            {
                sentences[sentence_i][j] = '\0';
                // i--;
                // printf("Free heap size: %i\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
                memmove(sentences[sentence_i], sentences[sentence_i]+sep_len, strlen(sentences[sentence_i]));
                sentence_i++;
 //               sentences[sentence_i] = (char*)heap_caps_malloc(sizeof_char * (max_sen_len + 1), MALLOC_CAP_8BIT);
                // temp_sentence[0] = '\0';
                j = 0;
            }
        }
        i++;
    }
    free(temp_substr);
//    return sentences;
}
