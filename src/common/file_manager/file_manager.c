#include "file_manager.h"

int fm_exists(struct internal_state * self, void * msg_file)
{
  // Point to data
  struct msg_file * msg = msg_file;
  // Format path
  char path[130];
  struct hash * h = (void*)msg->hash;
  snprintf(path, 130, "%s/%016lx%016lx%016lx%016lx", self->fpath, h->a, h->b, h->c, h->d);

  // Check if exists
  FILE * f;
  if (f = fopen(path, "rb"))
  {
    fclose(f);
    return 1;
  }

  return 0;
}

int fm_store(struct internal_state * self, void * msg_file)
{
  // Point to data
  struct msg_file * msg = msg_file;
  // Format path
  char path[130];
  struct hash * h = (void*)msg->hash;
  snprintf(path, 130, "%s/%016lx%016lx%016lx%016lx", self->fpath, h->a, h->b, h->c, h->d);
  // Open file
  FILE * f = fopen(path, "wb");
  // Check for errors
  if(!f) return 0;
  // Write file
  fwrite(msg->data, 1, 8192, f);
  // Close
  fclose(f);
  // Return success
  return 1;
}

int fm_read(struct internal_state * self, void * msg_file)
{
  // Point to data
  struct msg_file * msg = msg_file;
  // Format path
  char path[130];
  struct hash * h = (void*)msg->hash;
  snprintf(path, 130, "%s/%016lx%016lx%016lx%016lx", self->fpath, h->a, h->b, h->c, h->d);
  // Open file
  FILE * f = fopen(path, "rb");
  // Check for errors
  if(!f) return 0;
  // Read file
  int r = fread(msg->data, 1, 8192, f);
  // Close
  fclose(f);
  // Return success
  return 1;
}
