#include "system.h"
// template request header 
static const char* Html_MainPage =
                                "HTTP/1.1 %d %s\r\n"
                                "Content-Type: %s\r\n"
                                "Content-Length: %d\r\n"
                                "Connection: %s\r\n"
                                "\r\n"
                                "%s";


static void HttpHeaderFindValue(const char* HttpHeader, const char* key, char* r){
    
}

static void ParseQuery(const char* str, size_t len, struct HttpQuery* r){
    char* pC = str; // this variable can be replaced by str 
    char iC=0;
    u8 idx=0; // why would it be longer?
    while(idx < len &&(iC = pC[idx]) != '='){ //len -> delim character(?)
       r->key[idx] = iC; 
       idx++;
    }
    r->value[idx] ='\0';
    pC=&pC[idx]; //move by the processed chars
    pC++; //skip '='
    len -= (idx+1); // current character also counts as idx++
    idx =0;
    while(idx < len){
       r->value[idx] = pC[idx]; 
       idx++;
    }
    r->value[idx] ='\0';
}

enum TCPthread_states
ProcessRequest(const char* HttpReq, struct HttpRequest_t *r){
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
                                                   
        //Get does not function properly so no time to fix the rest only get requests for now
        LOG_UART(LOG_TRACE, buffer, NULL);
        if (strcmp(buffer,"GET")==0)  
            r->header.ReqType = REQ_GET;
        /*else if (strcmp(buffer,"POST")==0) 
            r->header.ReqType = REQ_POST;
        else if (strcmp(buffer,"PUT")==0)
            r->header.ReqType = REQ_PUT;*/
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
            r->header.ReqDir[(pOP-pC)+1] ='\0';
            LOG_UART(LOG_TRACE, r->header.ReqDir, NULL);
        }else{
            pOP = strstr(pC, " HTTP/1.1"); // no queries so process path   
            strncpy(r->header.ReqDir, pC, (pOP-pC)); //dont include space char 
            r->header.ReqDir[(pOP-pC)+1] ='\0';
            LOG_UART(LOG_TRACE, r->header.ReqDir, NULL);
        }

        /* process and add queries
           - double Linked list stores query values 
        */
        pC = pOP; // move active character after the path character 
        struct HttpQuery *cQuery = &r->queries; // currently processed query
	    struct xHeapStats stat;
        while(1){
            pC++; //skip the ? character to prevent infinite loop && pOP-pC = 1 
            if((pOP = strchr(pC,'?')) != NULL){ //multiple arguments
                ParseQuery(pC, (pOP-pC), cQuery);//10th addr-> '?'               
                if(cQuery->next == NULL){
                    if(stat.xSizeOfLargestFreeBlockInBytes < sizeof(struct HttpQuery)*2){
		                return DISCONNECT;		
	                }
                    cQuery->next = pvPortCalloc(1,sizeof(struct HttpQuery));
                    if(cQuery->next == NULL)
                       LOG_UART(LOG_ERROR,"FAILED TO ALLOCATE MEMORY FOR HTTP QUERY OBJECT", NULL); 
                    
                    cQuery->next->prev = cQuery;
                    cQuery = cQuery->next; //next query element shoudl get filled
                    r->QueryAmount++;
                    pC =pOP; //assign next value to current char*
                }
            }else{ //only 1 argument remains
                pOP = strstr(pC," HTTP/1.1"); //it also searches for space character
                if(pOP != 0){// found query 
                    ParseQuery(pC, (pOP-pC), cQuery);                
                    LOG_UART(LOG_DEBUG,LOG_ORIGIN("=== QUERIES ==="),LOG_printHttpQueries, &r->queries);
                }
                break;
            }
        }

        switch(r->header.ReqType){
            case REQ_GET:
                if(strcmp(r->header.ReqDir, "/") == 0){// home directory requested
                    const char* body = indexHtml;
                    size_t body_len = strlen(body); 
                    r->rSize =snprintf(r->response,sizeof(r->response), Html_MainPage,200,"OK",
                        "text/html",body_len, "Keep-Alive", body);
                    return SENDING; 
                }
                
               r->rSize = snprintf(r->response, sizeof(r->response),
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n"
                    "\r\n");
                return SENDING;
                
                //return valid http
                return RECEIVING;                         
                break;
            case REQ_POST:
                break;
            case REQ_PUT:
                break;
            case REQ_UNKNOWN:
                r->rSize = snprintf(r->response, sizeof(r->response),
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n"
                    "\r\n");
                return SENDING;
            default:
                break;
        }

    }else{
        // not HTTP request | request which can be parsed by system
        LOG_UART(LOG_TRACE, "NOT A HTTP HEADER", NULL);
        LOG_UART(LOG_TRACE, HttpReq, NULL);
        return DISCONNECT;	
    }   

    
}