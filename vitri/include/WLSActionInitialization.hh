/// \file optical/wls/include/WLSActionInitialization.hh
/// \brief Definition of the WLSActionInitialization class

#ifndef WLSActionInitialization_h
#define WLSActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class WLSDetectorConstruction;

class WLSActionInitialization : public G4VUserActionInitialization
{
  public:
    WLSActionInitialization(WLSDetectorConstruction*);
    ~WLSActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;

  private:
    WLSDetectorConstruction* fDetector = nullptr;
};

#endif
