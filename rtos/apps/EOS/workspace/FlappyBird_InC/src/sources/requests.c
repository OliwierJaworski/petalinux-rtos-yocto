#include "system.h"
// template request header 
static const char* Html_MainPage ="HTTP/1.1 %d %s\r\n \
        						Content-Type: %s\r\n \
								Content-Length: %d\r\n \
								Connection: %s\r\n \
								\r\n" ;


static void HttpHeaderFindValue(const char* HttpHeader, const char* key, char* r){
    
}

void ProcessRequest(const char* HttpReq, struct HttpRequest_t *r){
    //LOG_UART(LOG_TRACE,"DETERMENING REQUEST TYPE, CONTENT:",NULL);
    //LOG_UART(LOG_TRACE,HttpReq,NULL);
   
    char* pC= HttpReq;;// current index the begin of string
    char* pOP= NULL;// operation idx returns value of operation  
    char buffer[200];
    
    
    pOP = strchr(pC, 'HTTP/1.1'); //find delimiter which is space 
    if(pOP != NULL){         
        /* Find the request type
           example:
           - input : GET /?ss=0?disk="22" HTTP/1.1
           - output: GET
        */
        pOP = strchr(pC, ' '); //valid http so search first space before, it is the REST-method

        strncpy(buffer, HttpReq, (pOP-pC));   
                                                   
        LOG_UART(LOG_TRACE, buffer, NULL);
        if (strcmp(buffer,"GET")==0)  
            r->header.ReqType = REQ_GET;
        else if (strcmp(buffer,"POST")==0) 
            r->header.ReqType = REQ_POST;
        else if (strcmp(buffer,"PUT")==0)
            r->header.ReqType = REQ_PUT;
        else 
            r->header.ReqType = REQ_UNKNOWN;
         
        /* Find the path 
           example:
           - input : GET /?ss=0?disk="22" HTTP/1.1
           - output: / 
        */
        pOP++; // skip the space
        pC = pOP; //set the Current pointer to first character of path 
        pOP = strchr(pC, '?'); // check if request provides queries
        if(pOP){
            // if it has arguments process them
            strncpy(r->header.ReqDir, pC, (pOP-pC)); 
            LOG_UART(LOG_TRACE, r->header.ReqDir, NULL);
        }else{
            pOP = strstr(pC, "HTTP/1.1"); // no queries so process path   
            strncpy(r->header.ReqDir, pC, (pOP-pC-1)); 
            LOG_UART(LOG_TRACE, r->header.ReqDir, NULL);
        }
        /* process and add queries
        */

    }else{
        // not HTTP request | request which can be parsed by system
        LOG_UART(LOG_ERROR, "NOT A HTTP HEADER", NULL);
        return;
    }   

    switch(r->header.ReqType){
        case REQ_GET:
            break;
        case REQ_POST:
            break;
        case REQ_PUT:
            break;
        default:
            break;
    }
}