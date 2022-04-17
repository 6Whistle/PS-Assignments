CC = g++
OPT = -o
RUN1 = encoding
RUN2 = decoding
PATH1 = encoding.cpp
PATH2 = decoding.cpp

all : $(RUN1) $(RUN2)
    
encoding : $(PATH1)
    $(CC) $(OPT) $@ $^

decoding : $(PATH2)
    $(CC) $(OPT) $@ $^

clean : 
    rm -rf $(RUN1) $(RUN2)