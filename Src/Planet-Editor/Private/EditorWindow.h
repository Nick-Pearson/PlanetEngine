#pragma once

class EditorWindow
{
 public:
    virtual ~EditorWindow() {}

    virtual void Draw() = 0;
};