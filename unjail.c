#include "unjail.h"

#define X86_CR0_WP (1 << 16)


unsigned int long long __readmsr(unsigned long __register) {
	unsigned long __edx;
	unsigned long __eax;
	__asm__("rdmsr" : "=d"(__edx), "=a"(__eax) : "c"(__register));
	return (((unsigned int long long)__edx) << 32) | (unsigned int long long)__eax;
}

void  *unjail455(struct thread *td){

        struct ucred* cred;
        struct filedesc* fd;

        fd = td->td_proc->p_fd;
        cred = td->td_proc->p_ucred;

        void* kernel_base = &((uint8_t*)__readmsr(0xC0000082))[-0x3095D0];
        uint8_t* kernel_ptr = (uint8_t*)kernel_base;
        void** got_prison0 =   (void**)&kernel_ptr[0x10399B0];
        void** got_rootvnode = (void**)&kernel_ptr[0x21AFA30];

        cred->cr_uid = 0;
        cred->cr_ruid = 0;
        cred->cr_rgid = 0;
        cred->cr_groups[0] = 0;

        cred->cr_prison = *got_prison0;
        fd->fd_rdir = fd->fd_jdir = *got_rootvnode;

        // escalate ucred privs, needed for access to the filesystem ie* mounting & decrypting files
        void *td_ucred = *(void **)(((char *)td) + 304); // p_ucred == td_ucred

        // sceSblACMgrIsSystemUcred
        uint64_t *sonyCred = (uint64_t *)(((char *)td_ucred) + 96);
        *sonyCred = 0xffffffffffffffff;

        // sceSblACMgrGetDeviceAccessType
        uint64_t *sceProcType = (uint64_t *)(((char *)td_ucred) + 88);
        *sceProcType = 0x3801000000000013; // Max access

        // sceSblACMgrHasSceProcessCapability
        uint64_t *sceProcCap = (uint64_t *)(((char *)td_ucred) + 104);
        *sceProcCap = 0xffffffffffffffff; // Sce Process


        return 0;
}
