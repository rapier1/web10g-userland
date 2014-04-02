/*
 * Copyright (c) 2013 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this library;
 * if not, see http://opensource.org/licenses/MIT.
 *
 */
#include <estats/estats-int.h>
#include <estats/sockinfo-int.h>
#include <estats/sockinfo.h>


static estats_error* _estats_sockinfo_refresh_list(estats_sockinfo *);
static estats_error* _estats_sockinfo_get_tcp_list(struct estats_list* tcp_list, estats_agent* agent);
static estats_error* _estats_sockinfo_get_ino_list(struct estats_list* ino_list);
static estats_error* _estats_sockinfo_get_pid_list(struct estats_list* pid_list);
static estats_error* _estats_sockinfo_new_entry(estats_sockinfo_item** _sockinfo);
static void          _estats_sockinfo_free_entry(estats_sockinfo_item** sockinfo);

estats_error*
estats_sockinfo_new(estats_sockinfo** sinfo, estats_agent* agent)
{
    estats_error* err = NULL;

    ErrIf(sinfo == NULL, ESTATS_ERR_INVAL);

    Chk(Malloc((void**) sinfo, sizeof(estats_sockinfo)));
    memset((void*) *sinfo, 0, sizeof(estats_sockinfo));
    _estats_list_init(&((*sinfo)->list_head));
    (*sinfo)->agent = agent;

Cleanup:
    return err;
}

estats_error*
estats_sockinfo_get_list_head(estats_sockinfo_item** item, estats_sockinfo* sinfo)
{
    estats_error* err = NULL;
    struct estats_list* head;

    ErrIf(item == NULL || sinfo == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_sockinfo_refresh_list(sinfo));

    head = &(sinfo->list_head);
    *item = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_sockinfo_item, list);

Cleanup:
    return err;
}

static estats_error*
_estats_sockinfo_new_entry(estats_sockinfo_item** ci)
{
    estats_error* err = NULL; 

    ErrIf(ci == NULL, ESTATS_ERR_INVAL);                                               
    *ci = NULL;

    Chk(Malloc((void**) ci, sizeof(estats_sockinfo_item)));
    memset((void*) *ci, 0, sizeof(estats_sockinfo_item));

Cleanup:
    return err;
}

estats_error*
estats_sockinfo_item_next(estats_sockinfo_item** next, const estats_sockinfo_item* prev)
{
    estats_error* err = NULL;
    struct estats_list* l;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    l = prev->list.next;
    if (l == &(prev->sockinfo->list_head))
        *next = NULL;
    else
        *next = ESTATS_LIST_ENTRY(l, estats_sockinfo_item, list);

Cleanup:
    return err;
}

estats_sockinfo_item*
estats_sockinfo_item_return_next(const estats_sockinfo_item* prev)
{
    estats_error* err = NULL;
    estats_sockinfo_item* next = NULL;

    Chk(estats_sockinfo_item_next(&next, prev));

Cleanup:
    if (err) return NULL;
    else return next;
}

void
estats_sockinfo_free(estats_sockinfo** sockinfo)
{
    struct estats_list* pos;
    struct estats_list* tmp;

    if (sockinfo == NULL || *sockinfo == NULL)
        return;

    ESTATS_LIST_FOREACH_SAFE(pos, tmp, &((*sockinfo)->list_head)) {
        estats_sockinfo_item* item = ESTATS_LIST_ENTRY(pos, estats_sockinfo_item, list);
        _estats_list_del(pos);
        _estats_sockinfo_free_entry(&item);
    }

    Free((void**) sockinfo);
}

void
_estats_sockinfo_free_entry(estats_sockinfo_item** sockinfo)
{
    if (sockinfo == NULL || *sockinfo == NULL)
	return;

    Free((void**) sockinfo);
}

estats_error*
estats_sockinfo_get_cid(int* cid, const estats_sockinfo_item* ecl)
{
    estats_error* err = NULL;

    ErrIf(cid == NULL || ecl == NULL, ESTATS_ERR_INVAL);
    *cid = ecl->cid;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_pid(int* pid, const estats_sockinfo_item* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(pid == NULL || sockinfo == NULL, ESTATS_ERR_INVAL);
    *pid = sockinfo->pid;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_uid(int* uid, const estats_sockinfo_item* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(uid == NULL || sockinfo == NULL, ESTATS_ERR_INVAL);
    *uid = sockinfo->uid;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_state(int* state, const estats_sockinfo_item* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(state == NULL || sockinfo == NULL, ESTATS_ERR_INVAL);
    *state = sockinfo->state;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_cmdline(char** str, const estats_sockinfo_item* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(sockinfo == NULL, ESTATS_ERR_INVAL);
    Chk(Strdup(str, sockinfo->cmdline));

Cleanup:
    return err;
}

estats_error*
estats_sockinfo_get_connection_spec(struct estats_connection_spec* spec, const estats_sockinfo_item* item)
{
    estats_error* err = NULL;

    ErrIf(spec == NULL || item == NULL, ESTATS_ERR_INVAL);

    *spec = item->spec; // struct copy

Cleanup:
    return err;
}

static estats_error*
_estats_sockinfo_refresh_list(estats_sockinfo* sinfo)
{
    estats_error* err = NULL;
    struct estats_list tcp_head;
    struct estats_list ino_head;
    struct estats_list pid_head;
    struct estats_list* tcp_pos;
    struct estats_list* ino_pos;
    struct estats_list* pid_pos;
    struct estats_list* tmp;
    estats_sockinfo_item* newcl;
    int dif;
    int tcp_entry, fd_entry;

    /* associate cid with address */ 
    Chk(_estats_sockinfo_get_tcp_list(&tcp_head, sinfo->agent));

    /* associate address with ino */
    Chk(_estats_sockinfo_get_ino_list(&ino_head));

    /* associate ino with pid */
    Chk(_estats_sockinfo_get_pid_list(&pid_head));

    /* collate above */

    ESTATS_LIST_FOREACH(tcp_pos, &tcp_head) { 

        estats_sockinfo_item* tcp_ent = ESTATS_LIST_ENTRY(tcp_pos, estats_sockinfo_item, list);
	tcp_entry = 0;

        ESTATS_LIST_FOREACH(ino_pos, &ino_head) {

            estats_sockinfo_item* ino_ent = ESTATS_LIST_ENTRY(ino_pos, estats_sockinfo_item, list);
            
	    Chk(estats_connection_spec_compare(&dif, &ino_ent->spec, &tcp_ent->spec));

	    if (!dif) {
	       	tcp_entry = 1;
	       	fd_entry = 0;

                ESTATS_LIST_FOREACH(pid_pos, &pid_head) {

                    estats_sockinfo_item* pid_ent = ESTATS_LIST_ENTRY(pid_pos, estats_sockinfo_item, list);

		    if(pid_ent->ino == ino_ent->ino) { //then create entry 
			fd_entry = 1;

                        Chk(_estats_sockinfo_new_entry(&newcl));

			newcl->pid = pid_ent->pid; 

                        strlcpy(newcl->cmdline, pid_ent->cmdline, sizeof(pid_ent->cmdline));
                        newcl->uid = ino_ent->uid;
		       	newcl->state = ino_ent->state;

			newcl->cid = tcp_ent->cid;

			newcl->addrtype = tcp_ent->addrtype; 

                        newcl->spec = tcp_ent->spec; // struct copy

                        newcl->sockinfo = sinfo;

                        _estats_list_add_tail(&(newcl->list), &(sinfo->list_head));
		    }
	       	}
	       	if(!fd_entry) { // add entry w/out cmdline 

                        Chk(_estats_sockinfo_new_entry(&newcl));

			newcl->pid = 0;
			newcl->uid = ino_ent->uid;
		       	newcl->state = ino_ent->state;

			newcl->cid = tcp_ent->cid;
			newcl->addrtype = tcp_ent->addrtype; 

                        newcl->spec = tcp_ent->spec; // struct copy
                        
                        newcl->sockinfo = sinfo;

                        strlcpy(newcl->cmdline, "\0", 1);
                        
                        _estats_list_add_tail(&(newcl->list), &(sinfo->list_head));
	       	}
	    }
	}
	if(!tcp_entry) { // then connection has vanished; add residual cid info
			 // (only for consistency with entries in /proc) 

            Chk(_estats_sockinfo_new_entry(&newcl));

	    newcl->cid = tcp_ent->cid; 
	    newcl->addrtype = tcp_ent->addrtype; 

            newcl->spec = tcp_ent->spec; // struct copy

            newcl->sockinfo = sinfo;

            strlcpy(newcl->cmdline, "\0", 1);

            _estats_list_add_tail(&(newcl->list), &(sinfo->list_head));
       	}
    }

Cleanup:
    ESTATS_LIST_FOREACH_SAFE(tcp_pos, tmp, &tcp_head) {
        estats_sockinfo_item* tcp_ent = ESTATS_LIST_ENTRY(tcp_pos, estats_sockinfo_item, list);
        _estats_list_del(tcp_pos);
        free(tcp_ent);
    }
    ESTATS_LIST_FOREACH_SAFE(ino_pos, tmp, &ino_head) {
        estats_sockinfo_item* ino_ent = ESTATS_LIST_ENTRY(ino_pos, estats_sockinfo_item, list);
        _estats_list_del(ino_pos);
        free(ino_ent);
    }
    ESTATS_LIST_FOREACH_SAFE(pid_pos, tmp, &pid_head) {
        estats_sockinfo_item* pid_ent = ESTATS_LIST_ENTRY(pid_pos, estats_sockinfo_item, list);
        _estats_list_del(pid_pos);
        free(pid_ent);
    }

    return err;
}


static estats_error*
_estats_sockinfo_get_tcp_list(struct estats_list* head, estats_agent* agent)
{
    estats_error* err = NULL; 
    estats_connection* conn = NULL;

    ErrIf(head == NULL, ESTATS_ERR_INVAL);
    _estats_list_init(head);

    Chk(estats_agent_get_connection_head(&conn, agent));

    while (conn != NULL) { 
	estats_sockinfo_item* entry = NULL; 

        Chk(_estats_sockinfo_new_entry(&entry));

	Chk(estats_connection_get_cid(&entry->cid, conn));
       	Chk(estats_connection_get_connection_spec(&entry->spec, conn));
	Chk(estats_connection_get_addrtype(&entry->addrtype, conn));

        _estats_list_add_tail(&entry->list, head);

	Chk(estats_connection_next(&conn, conn));
    }

Cleanup:
    return err;
}


static estats_error*
_estats_sockinfo_get_ino_list(struct estats_list* head)
{ 
    estats_error* err = NULL;
    FILE *file = NULL;
    FILE *file6 = NULL;
    char buf[256];
    int scan;
    struct in6_addr in6;

    ErrIf(head == NULL, ESTATS_ERR_INVAL);
    _estats_list_init(head);

    file = fopen("/proc/net/tcp", "r");
    file6 = fopen("/proc/net/tcp6", "r");

    if (file) {
	estats_sockinfo_item* sockinfo;
	uint32_t srcAddr;
	uint16_t srcPort;
	uint32_t dstAddr;
	uint16_t dstPort;

       	while (fgets(buf, sizeof(buf), file) != NULL) {

	    Chk(_estats_sockinfo_new_entry(&sockinfo));

	    if ((scan = sscanf(buf,
			    "%*u: %x:%hx %x:%hx %x %*x:%*x %*x:%*x %*x %u %*u %lu",
			    (uint32_t *) &(sockinfo->spec.src_addr),
			    (uint16_t *) &(sockinfo->spec.src_port),
			    (uint32_t *) &(sockinfo->spec.dst_addr),
			    (uint16_t *) &(sockinfo->spec.dst_port),
			    (int *) &(sockinfo->state),
			    (uid_t *) &(sockinfo->uid),
			    (ino_t *) &(sockinfo->ino))) == 7) { 

		sockinfo->addrtype = ESTATS_ADDRTYPE_IPV4;

                _estats_list_add_tail(&sockinfo->list, head);
	    } else {
	       	free(sockinfo);
	    }
       	}
       	fclose(file);
    }

    if (file6) { 
	estats_sockinfo_item* sockinfo;
	char srcAddr[INET6_ADDRSTRLEN];
	uint16_t srcPort;
	char dstAddr[INET6_ADDRSTRLEN];
	uint16_t dstPort;

	while (fgets(buf, sizeof(buf), file6) != NULL) {

	    Chk(_estats_sockinfo_new_entry(&sockinfo));

	    if ((scan = sscanf(buf,
			    "%*u: %64[0-9A-Fa-f]:%hx %64[0-9A-Fa-f]:%hx %x %*x:%*x %*x:%*x %*x %u %*u %u", 
			    (char *) &srcAddr,
			    (uint16_t *) &(sockinfo->spec.src_port),
			    (char *) &dstAddr,
			    (uint16_t *) &(sockinfo->spec.dst_port),
			    (int *) &(sockinfo->state),
			    (uid_t *) &(sockinfo->uid),
			    (pid_t *) &(sockinfo->ino))) == 7) { 

		sscanf(srcAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]); 

                memcpy(&(sockinfo->spec.src_addr), &in6.s6_addr, 16);

		sscanf(dstAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]);

                memcpy(&(sockinfo->spec.dst_addr), &in6.s6_addr, 16);

		sockinfo->addrtype = ESTATS_ADDRTYPE_IPV6;

                _estats_list_add_tail(&sockinfo->list, head);
	    } else { 
		free(sockinfo);
	    }
       	}
       	fclose(file6);
    } 

Cleanup:
    return err;
}


static estats_error*
_estats_sockinfo_get_pid_list(struct estats_list* head)
{ 
    estats_error* err = NULL;
    estats_sockinfo_item* ecl;
    DIR *dir, *fddir; 
    struct dirent *direntp, *fddirentp;
    pid_t pid;
    char path[PATH_MAX];
    char buf[256];
    struct stat st;
    int stno;
    FILE* file;

    ErrIf(head == NULL, ESTATS_ERR_INVAL);
    _estats_list_init(head);
  
    Chk(Opendir(&dir, "/proc"));

    while ((direntp = readdir(dir)) != NULL) {
       	if ((pid = atoi(direntp->d_name)) != 0)
       	{
	    sprintf(path, "%s/%d/%s/", "/proc", pid, "fd"); 
	    if ((fddir = opendir(path)) != NULL)  //else lacks permissions 
	    { 
		while ((fddirentp = readdir(fddir)) != NULL) 
		{ 
		    strcpy(buf, path);
		    strcat(buf, fddirentp->d_name); 
		    stno = stat(buf, &st); 
		    if (S_ISSOCK(st.st_mode)) { // add new list entry

			sprintf(buf, "/proc/%d/status", pid);

			if ((file = fopen(buf, "r")) == NULL) continue;

			if (fgets(buf, sizeof(buf), file) == NULL)
			    goto FileCleanup; 
       	
	                Chk(_estats_sockinfo_new_entry(&ecl));

			if (sscanf(buf, "Name: %16s\n", ecl->cmdline) != 1) {
			    Free((void**) &ecl);
			    goto FileCleanup;
		       	}

			ecl->ino = st.st_ino;
		       	ecl->pid = pid;
                        _estats_list_add_tail(&ecl->list, head);

FileCleanup:
			fclose(file); 
		    }
	       	}
	       	closedir(fddir);
	    }
       	}
    }
    closedir(dir);

Cleanup:
    return err;
}
