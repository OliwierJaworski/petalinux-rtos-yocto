#include "gpio.h"

gpio::gpio(const char* fd_path){
    set_fd_handle(fd_path);
    map_Vsize(fd_path);
    map_Vaddr(fd_path);
}

gpio::~gpio(){
    munmap(Vaddr, Vsize);
    close(fd);
}

int
gpio::set_fd_handle(const char* fd_path){
    fd = open(fd_path, O_RDWR | O_SYNC);
    if(fd == -1){
        fprintf(stderr,"failed to open character device at:%s with error code %s\n",fd_path, strerror(errno));
            exit(1);
    }
    return 0;
}

int
gpio::map_Vsize(const char* fd_path) {
    const char* devName = strrchr(fd_path, '/');
    if (!devName || *(devName + 1) == '\0') {
        fprintf(stderr, "failed to parse device name from: %s\n", fd_path);
        exit(1);
    }
    devName++;  // skip the '/'

    char buf[128];
    snprintf(buf, sizeof(buf), "/sys/class/uio/%s/maps/map0/size", devName);
    printf("[DEBUG] sysfs path = %s\n", buf);

    FILE* f = fopen(buf, "r");
    if (!f) {
        fprintf(stderr, "fopen failed for %s: %s\n", buf, strerror(errno));
        exit(1);
    }

    if (fscanf(f, "%x", &Vsize) != 1) {
        fclose(f);
        fprintf(stderr, "failed to parse Vsize from %s\n", buf);
        exit(1);
    }
    fclose(f);
    return 0;
}

int
gpio::map_Vaddr(const char* fd_path){
    printf("[DEBUG] Mapping fd_path=%s\n", fd_path);
    printf("[DEBUG] fd=%d Vsize=0x%x (%u)\n", fd, Vsize, Vsize);

    Vaddr = (uint32_t*)mmap(NULL,Vsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if( (void*) Vaddr == MAP_FAILED ){
        fprintf(stderr,"failed to map:%s\n",fd_path);
        exit(1);
    }
    printf("[DEBUG] mmap succeeded. Vaddr=%p\n", (void*)Vaddr);
    return 0;
}

