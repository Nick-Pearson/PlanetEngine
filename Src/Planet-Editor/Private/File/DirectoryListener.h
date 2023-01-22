#pragma once

class DirectoryListener
{
 public:
    virtual void OnFileAdded() = 0;
    virtual void OnFileUpdated() = 0;
    virtual void OnFileRemoved() = 0;
};
