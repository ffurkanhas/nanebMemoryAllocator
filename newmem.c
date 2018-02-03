#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "newmem.h"

mem_node* allHead;
int freeMem;

int Mem_Init(int sizeOfRegion){
	int pageSize = getpagesize(); //Get system pageSize
	if(sizeOfRegion % pageSize != 0){ // must be exactly times the page size
		sizeOfRegion += pageSize - (sizeOfRegion % pageSize);
	}
	int fd = open("/dev/zero", O_RDWR);
	mem_node *ptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE,MAP_PRIVATE, fd, 0); //ToplamBoyut sayfa büyüklüğünün katı olmalı
	if(ptr == MAP_FAILED){ 
		perror("mmap"); return -1; 
	}
	allHead=ptr;
	ptr->size=sizeOfRegion-sizeof(mem_node);
	ptr->next=NULL;
	freeMem = ptr->size;
	close(fd);
	return 0;
}

void *Mem_Alloc(int size){
	mem_node* head=allHead;
	mem_node* tmp=NULL;
	if(size % 8 != 0){
			size += 8 - (size % 8);
	}
	while((head!=NULL)){
		if(size<=head->size){
			mem_node* temp=head->next;
			if(head->size-size>=sizeof(mem_node)){
			head->next=(void *)head+size+sizeof(mem_node);
			head->next->size=head->size-(size+sizeof(mem_node));
			head->next->next=temp;
		 	head->size=0;
			tmp=head;
			head=head->next;
			}
			return ((void*)tmp+sizeof(mem_node));
		}
		else{
			head=head->next;
		}
	}
	printf("SIZE ERROR\n");
	return NULL;
}

int Mem_Free(void *ptr){
	if(ptr==NULL)
		return -1;
	mem_node* head=allHead;
	while(head!=NULL){
		if(((void*)head+sizeof(mem_node))==ptr){
			if(head->next==NULL){
				head->size=(int)((void*)allHead+freeMem-(void*)head)-sizeof(mem_node);
				return 0;
			}
			else{
				if(head->next->size!=0){
					head->size=(int)((void*)head->next-(void*)head)+head->next->size;
					head->next=(head->next)->next;
				}
				else
					head->size=(int)((void*)head->next-(void*)head)-sizeof(mem_node);
				return 0;
			}
		}
		head=head->next;
	   }
	return -1;
}

void Mem_Dump(){
	mem_node* head=allHead;
	while(head!=NULL){
		printf("Adres: %p Free Size: %d\n",(void*)head+sizeof(mem_node),head->size);
		head=head->next;
	}
}

int main (int argc, char **argv) {
	int bellek = Mem_Init (4096); //ornek
	//printf ("Mem_Init %d byte kadar bellek ayirdi.\n\n", bellek);
	Mem_Dump(); // Burada ayirdiginiz bellek boyutu ve yerini gormelisiniz
	//Unutmayin 4096 byte’in bir kiminda ustveri duruyor olacak.
	void *ptr = Mem_Alloc (1); // 1 bytelik bellek ayir ve 5 degerini kaydet/
	printf ("1 bytelik bellegi %p adresinde ayirdim\n", ptr);
	*((int *) ptr) = 50;
	Mem_Dump();
	printf("********** %d\n",*((int *) ptr));

	void *ptr2 = Mem_Alloc (4000); // 4000 bytelik bellek ayir
	printf ("4000 bytelik bellegi %p adresinde ayirdim\n", ptr2);
	Mem_Dump();

	void *ptr3 = Mem_Alloc (1000); // Burada bir hata almayi beklemelisiniz.
	printf ("1000 bytelik bellegi %p adresinde ayirdim\n", ptr3);
	printf ("Mem_Alloc a %p\n", ptr3);
	Mem_Dump();

	void *ptr4 = Mem_Alloc (8); // 16 bytelik bellek ayir
	printf ("1000 bytelik bellegi %p adresinde ayirdim\n", ptr4);
	printf ("Mem_Alloc a %p\n", ptr4);
	Mem_Dump();

	printf ("Ilk blok serbest birakiliyor\n");
	if (Mem_Free(ptr) == -1) {
	printf("Hata olustu\n");
	}
	Mem_Dump(); // Iki serbest blogun bilgileri (adres ve boyut)

	printf ("Son blok serbest birakiliyor\n");
	if (Mem_Free(ptr4) == -1) {
	printf("Hata olustu\n");
	}
	Mem_Dump(); 

	printf ("Ayrilmamis blok serbest birakiliyor.\n");
	if (Mem_Free(ptr2 + 10) == -1) { //Ayrilmamis blogu serbest birakmaya
	//calisiyoruz, bir islem olmamali
	printf("Hatali adreste islem\n");}
	Mem_Dump();
}