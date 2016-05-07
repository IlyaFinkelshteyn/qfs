/*
 * $Id$
 *
 * \file Checkpoint.h
 * \brief KFS checkpointer
 * \author Blake Lewis (Kosmix Corp.)
 *
 * Copyright 2008-2012 Quantcast Corp.
 * Copyright 2006-2008 Kosmix Corp.
 *
 * This file is part of Kosmos File System (KFS).
 *
 * Licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 */
#if !defined(KFS_CHECKPOINT_H)
#define KFS_CHECKPOINT_H

#include <string>

#include "kfstypes.h"
#include "util.h"

namespace KFS {
using std::string;

/*!
 * \brief keeps track of checkpoint status
 *
 * This class records the current state of the metadata server
 * with respect to checkpoints---the name of the checkpoint file,
 * whether the CP is running, whether the server has any recent
 * updates that would make a new CP worthwhile, etc.
 *
 * Writing out a checkpoint involves walking the leaves of the
 * metatree and saving them to disk.  The on-disk checkpoint file stores
 * the name of the log that contains all the operations after the checkpoint
 * was taken.  For failure recovery, there is a notion of a "LATEST" checkpoint
 * file (created via a hardlink) that identifies the checkpoint that should be
 * used for restore purposes.
 *
 */
class Checkpoint
{
public:
    static const int VERSION = 1;
    Checkpoint(const string& d = string())
        : cpdir(d),
          writesync(true),
          writebuffersize(16 << 20),
          cpname()
        {}
    void setCPDir(const string& d)
        { cpdir = d; }
    const string name() const { return cpname; }
    int write(
        const string& logname,
        seq_t         committedseq,
        int64_t       errchksum);        //!< do the actual work
    bool getWriteSyncFlag() const { return writesync; }
    void setWriteSyncFlag(bool flag) { writesync = flag; }
    size_t getWriteBufferSize() const { return writebuffersize; }
    void setWriteBufferSize(size_t size) { writebuffersize = size; }
    string cpfile(seq_t highest) const //!< generate the next file name
        { return makename(cpdir, "chkpt", highest); }
private:
    string  cpdir;       //!< dir for CP files
    bool    writesync;
    size_t  writebuffersize;
    string  cpname;

    int write_leaves(ostream& os);
private:
    // No copy.
    Checkpoint(const Checkpoint&);
    Checkpoint& operator=(const Checkpoint&);
};

extern const string& CPDIR;        //!< directory for CP files
extern const string& LASTCP;       //!< most recent CP file (link)

extern Checkpoint& cp;
extern void checkpointer_setup_paths(const string &cpdir);

}

#endif // !defined(KFS_CHECKPOINT_H)
