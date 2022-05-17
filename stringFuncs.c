int getNextSpaceBar(char* text, int since){
        if(text[since]==' '){
                since++;
        }
        while(text[since] != ' '){
                since++;
        }
        return since;
}


//return indeks spacji po tym slowie
int getNextNick(char* input, char* output, int since){
        //szukanie pierwszej litery
        while(input[since] == ' '){
                since++;
        }

        int length=1;
        while(input[since+length] != ' ' && input[since+length] != '\0'){
                length++;
        }

        for(int i=0;i<length;i++){
                output[i]=input[since+i];
        }

        return since+length;
}


