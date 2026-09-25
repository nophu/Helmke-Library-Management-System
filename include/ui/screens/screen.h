#pragma once

class Screen {
public:
    virtual ~Screen() = default;
    virtual void onShow() {}
    virtual void onClose() {}
    virtual void update(float deltaTime) {}
    virtual void render() = 0;
};