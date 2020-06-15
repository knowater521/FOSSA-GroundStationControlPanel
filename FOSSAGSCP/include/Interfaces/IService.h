#ifndef ISERVICE_H
#define ISERVICE_H

class IService
{
public:
    virtual ~IService() {}
private:
    virtual void Initialize() = 0;
    virtual void Finialize() = 0;
};

#endif // ISERVICE_H
