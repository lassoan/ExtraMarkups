/*==============================================================================

  Copyright (c) The Intervention Centre
  Oslo University Hospital, Oslo, Norway. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Rafael Palomar (The Intervention Centre,
  Oslo University Hospital) and was supported by The Research Council of Norway
  through the ALive project (grant nr. 311393).

==============================================================================*/

#include "vtkMRMLMarkupsDiskNode.h"
#include "vtkMRMLMeasurementDisk.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkCollection.h>

#include <vtkMRMLSliceNode.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsDiskNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsDiskNode::vtkMRMLMarkupsDiskNode()
{
  /*
   * Point 0 : always the center.
   */
  this->MaximumNumberOfControlPoints = 3;
  this->RequiredNumberOfControlPoints = 3;
  
  auto addLengthMeasurement = [&] (const char * name, bool enabled)
  {
    vtkNew<vtkMRMLMeasurementDisk> measurement;
    measurement->SetUnits("mm");
    measurement->SetPrintFormat("%-#4.4g%s");
    measurement->SetName(name);
    measurement->SetInputMRMLNode(this);
    measurement->SetEnabled(enabled);
    this->Measurements->AddItem(measurement);
  };
  
  addLengthMeasurement("innerRadius", true);
  addLengthMeasurement("outerRadius", true);
  addLengthMeasurement("width", false);
  
  auto addAreaMeasurement = [&](const char * name) {
    vtkNew<vtkMRMLMeasurementDisk> measurement;
    measurement->SetUnits("cm2");
    measurement->SetDisplayCoefficient(0.01);
    measurement->SetPrintFormat("%-#4.4g%s");
    measurement->SetName(name);
    measurement->SetInputMRMLNode(this);
    measurement->SetEnabled(false);
    this->Measurements->AddItem(measurement);
  };
  
  addAreaMeasurement("area");
  addAreaMeasurement("innerArea");
  addAreaMeasurement("outerArea");
}

//--------------------------------------------------------------------------------
vtkMRMLMarkupsDiskNode::~vtkMRMLMarkupsDiskNode()=default;

//----------------------------------------------------------------------------
void vtkMRMLMarkupsDiskNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsDiskNode::ResliceToDiskPlane()
{
  if (!this->ResliceNode)
  {
    return;
  }
  vtkMRMLSliceNode * resliceNode = vtkMRMLSliceNode::SafeDownCast(this->ResliceNode);
  if (!resliceNode)
  {
    return;
  }
  double rasP1[3] = { 0.0 };
  double rasP2[3] = { 0.0 };
  double rasP3[3] = { 0.0 };
  double rasNormal[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  this->GetNthControlPointPositionWorld(1, rasP2);
  this->GetNthControlPointPositionWorld(2, rasP3);
  
  // Relative to rasP1 (center)
  double rRasP2[3] = { rasP2[0] - rasP1[0], rasP2[1] - rasP1[1], rasP2[2] - rasP1[2] };
  double rRasP3[3] = { rasP3[0] - rasP1[0], rasP3[1] - rasP1[1], rasP3[2] - rasP1[2] };
  
  vtkMath::Cross(rRasP2, rRasP3, rasNormal);
  if (rasNormal[0] == 0.0 && rasNormal[1] == 0.0 && rasNormal[2] == 0.0)
  {
    return;
  }
  resliceNode->SetSliceToRASByNTP(
    rasNormal[0], rasNormal[1], rasNormal[2],
    rasP2[0], rasP2[1], rasP2[2],
    rasP1[0], rasP1[1], rasP1[2],
    0);
  resliceNode->UpdateMatrices();
}

//----------------------------API only----------------------------------------
void vtkMRMLMarkupsDiskNode::SetInnerRadius(double radius)
{
  if (radius <= 0)
  {
    vtkErrorMacro("Radius must be greater than zero.");
    return;
  }
  double closestPoint[3] = { 0.0 };
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!this->DescribePointsProximity(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  if (radius >= outerRadius)
  {
    vtkErrorMacro("Inner radius must be less than outer radius.");
    return;
  }
  double rasP1[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  
  const double difference = radius - innerRadius;
  double closestPointShifted[3] = { 0.0 };
  this->FindLinearCoordinateByDistance(rasP1, closestPoint, closestPointShifted, difference);
  
  this->SetNthControlPointPositionWorld(this->GetClosestControlPointIndexToPositionWorld(closestPoint), closestPointShifted);
}

//----------------------------API only----------------------------------------
void vtkMRMLMarkupsDiskNode::SetOuterRadius(double radius)
{
  if (radius <= 0)
  {
    vtkErrorMacro("Radius must be greater than zero.");
    return;
  }
  double closestPoint[3] = { 0.0 };
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!this->DescribePointsProximity(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  if (radius <= innerRadius)
  {
    vtkErrorMacro("Outer radius must be greater than inner radius.");
    return;
  }
  double rasP1[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  
  const double difference = radius - outerRadius;
  double farthestPointShifted[3] = { 0.0 };
  this->FindLinearCoordinateByDistance(rasP1, farthestPoint, farthestPointShifted, difference);
  
  this->SetNthControlPointPositionWorld(this->GetClosestControlPointIndexToPositionWorld(farthestPoint), farthestPointShifted);
}

//----------------------------------------------------------------------
bool vtkMRMLMarkupsDiskNode::DescribePointsProximity(double * closestPoint, double * farthestPoint,
                                                     double& innerRadius, double& outerRadius)
{
  if (this->GetNumberOfDefinedControlPoints(true) != 3)
  {
    return false;
  }
  double p1[3] = { 0.0 }; // center
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, p1);
  this->GetNthControlPointPositionWorld(1, p2);
  this->GetNthControlPointPositionWorld(2, p3);
  
  double distance2 = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  double distance3 = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p3));
  
  if (distance2 <= distance3)
  {
    closestPoint[0] = p2[0];
    closestPoint[1] = p2[1];
    closestPoint[2] = p2[2];
    farthestPoint[0] = p3[0];
    farthestPoint[1] = p3[1];
    farthestPoint[2] = p3[2];
    innerRadius = distance2;
    outerRadius = distance3;
  }
  else
  {
    closestPoint[0] = p3[0];
    closestPoint[1] = p3[1];
    closestPoint[2] = p3[2];
    farthestPoint[0] = p2[0];
    farthestPoint[1] = p2[1];
    farthestPoint[2] = p2[2];
    innerRadius = distance3;
    outerRadius = distance2;
  }
  return true;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsDiskNode::FindLinearCoordinateByDistance(const double * p1, const double * p2,
                                                            double * result, const double difference)
{
  // Relative to p1, itself placed at origin. Simplifies reasoning.
  const double rp2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  const double factor = (1.0 + (difference / lineLength));
  result[0] = p1[0] + (rp2[0] * factor);
  result[1] = p1[1] + (rp2[1] * factor);
  result[2] = p1[2] + (rp2[2] * factor);
}
