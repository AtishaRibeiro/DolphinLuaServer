#include "CommonTypes.h"
#include "DolphinAddress.h"
#include "GameObject.h"

#include <map>
#include <vector>

struct KartPointers {
  DolphinAddress KartSettings;
  DolphinAddress KartState;
  DolphinAddress KartBody;
  DolphinAddress KartSuspensions;
  DolphinAddress KartTires;
  DolphinAddress PlayerModel;
  DolphinAddress KartSub;
  DolphinAddress KartSound;
  DolphinAddress Shadow;
  DolphinAddress KartCamera;
  DolphinAddress KartMove;
  DolphinAddress KartAction;
  DolphinAddress KartCollide;
};

struct KartPart {
  void update();

  DolphinAddress address;
  std::string modelName;
  Vec3 position;
  Matrix33 rotation;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(KartPart, modelName, position, rotation)
};

struct Tire {

  Vec3 position;
  Matrix33 rotation;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Tire, position, rotation)
};

class Player : public GameObject {
public:
  Player() {}
  Player(Pointers pointers, std::shared_ptr<PointerCache> pointerCache)
      : GameObject(pointers, pointerCache) {}
  void update() override;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Player, position, rotation, tires, kartParts)

private:
  void updateKartPointers(const DolphinAddress playerAddress);
  void updatePlayerRotation();
  void updateTireRotation(const int index, const DolphinAddress address);

  std::string getModelName(const DolphinAddress mdlPointer);
  /// Fill mKartPartsMap with all KartParts and their corresponding addresses.
  /// All KartParts are linked to eachother so once we have one part we can use
  /// that one to find all the others.
  /// `startPart` The part that is used as an entry to the linked parts.
  void setKartParts(const DolphinAddress startPart);

  DolphinAddress mPositionAddress;
  DolphinAddress mPhysicsAddress;
  KartPointers mKartPointers;

  // JSON
  Vec3 position;
  Matrix33 rotation;
  std::vector<Tire> tires;
  // Should be one map, but this can not be serialized trivially so we have a
  // duplicate vector just for serialization.
  std::map<DolphinAddress, KartPart> mKartPartsMap;
  std::vector<KartPart> kartParts;
};
