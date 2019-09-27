#ifndef ExampleHitDATA_H
#define ExampleHitDATA_H




/** @class ExampleHitData
 *  Example Hit
 *  @author: B. Hegner
 */

class ExampleHitData {
public:
  unsigned long long cellID;  ///< cellID
  double x;  ///< x-coordinate
  double y;  ///< y-coordinate
  double z;  ///< z-coordinate
  double energy;  ///< measured energy deposit
};


#endif
