#pragma once

class CBlender_accum_reflected : public IBlender
{
public:
    const char* getComment() override { return "INTERNAL: accumulate reflected light"; }
    bool canBeDetailed() override { return FALSE; }
    bool canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_reflected();
    virtual ~CBlender_accum_reflected();
};

class CBlender_accum_reflected_msaa : public IBlender
{
public:
    const char* getComment() override { return "INTERNAL: accumulate reflected light"; }
    bool canBeDetailed() override { return FALSE; }
    bool canBeLMAPped() override { return FALSE; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_reflected_msaa();
    virtual ~CBlender_accum_reflected_msaa();

    virtual void SetDefine(const char* Name, const char* Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    const char* Name;
    const char* Definition;
};
