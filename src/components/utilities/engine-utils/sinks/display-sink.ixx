export module displaysink;

import cameraconstructs;

export class DisplaySink
{
public:
  virtual ~DisplaySink() noexcept = default;
  virtual void present(const DisplayFrame& frame) = 0;
  virtual bool isSinkOpen() const noexcept = 0;
};

