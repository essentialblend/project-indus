export module i_indus;

export class IndusEngine
{

};


export class IEngineSystem abstract
{

};

export class IEngineComponent abstract
{

};

export class IEngineUtility abstract
{

};


export class IRenderer : public IEngineSystem
{

};

export class ICamera abstract : public IEngineComponent
{

};

export class IOverlay abstract : public IEngineComponent
{

};

export class IWindow abstract : public IEngineComponent
{

};

export class IImage abstract : public IEngineComponent
{

};



