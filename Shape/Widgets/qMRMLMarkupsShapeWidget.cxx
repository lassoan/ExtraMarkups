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

#include "qMRMLMarkupsShapeWidget.h"
#include "ui_qMRMLMarkupsShapeWidget.h"

// MRML Nodes includes
#include "vtkMRMLMarkupsShapeNode.h"

// VTK includes
#include <vtkWeakPointer.h>

#include <qSlicerCoreApplication.h>

// --------------------------------------------------------------------------
class qMRMLMarkupsShapeWidgetPrivate:
  public Ui_qMRMLMarkupsShapeWidget
{
  Q_DECLARE_PUBLIC(qMRMLMarkupsShapeWidget);

protected:
  qMRMLMarkupsShapeWidget* const q_ptr;

public:
  qMRMLMarkupsShapeWidgetPrivate(qMRMLMarkupsShapeWidget* object);
  void setupUi(qMRMLMarkupsShapeWidget*);

  vtkWeakPointer<vtkMRMLMarkupsShapeNode> MarkupsShapeNode;
};

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidgetPrivate::
qMRMLMarkupsShapeWidgetPrivate(qMRMLMarkupsShapeWidget* object)
  : q_ptr(object)
{

}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidgetPrivate::setupUi(qMRMLMarkupsShapeWidget* widget)
{
  Q_Q(qMRMLMarkupsShapeWidget);

  this->Ui_qMRMLMarkupsShapeWidget::setupUi(widget);
  this->shapeCollapsibleButton->setVisible(false);
  this->shapeCollapsibleButton->setCollapsed(true);
  this->shapeNameComboBox->addItem("Sphere");
  this->shapeNameComboBox->addItem("Ring");
  this->shapeNameComboBox->addItem("Disk");
  this->radiusModeComboBox->addItem("Centered");
  this->radiusModeComboBox->addItem("Circumferential");
  this->drawModeComboBox->addItem("Projection");
  this->drawModeComboBox->addItem("Intersection");
  this->resliceInputSelector->setMRMLScene(widget->mrmlScene());
  
  QObject::connect(this->shapeNameComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onShapeChanged(int)));
  QObject::connect(this->radiusModeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onRadiusModeChanged()));
  QObject::connect(this->drawModeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onDrawModeChanged()));
  QObject::connect(this->resolutionSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onResolutionChanged(double)));
  QObject::connect(this->resliceInputSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
                   q, SLOT(onResliceNodeChanged(vtkMRMLNode*)));
  QObject::connect(this->reslicePushButton, SIGNAL(clicked()),
                   q, SLOT(onResliceButtonClicked()));
}

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidget::
qMRMLMarkupsShapeWidget(QWidget *parent)
  : Superclass(parent),
    d_ptr(new qMRMLMarkupsShapeWidgetPrivate(this))
{
  this->setMRMLScene(qSlicerCoreApplication::application()->mrmlScene());
  this->setup();
}

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidget::~qMRMLMarkupsShapeWidget() = default;

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::setup()
{
  Q_D(qMRMLMarkupsShapeWidget);
  d->setupUi(this);
}
// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLMarkupsShapeWidget);

  if (!this->canManageMRMLMarkupsNode(d->MarkupsShapeNode))
    {
    d->shapeCollapsibleButton->setVisible(false);
    return;
    }

  d->shapeCollapsibleButton->setVisible(true);
}


//-----------------------------------------------------------------------------
bool qMRMLMarkupsShapeWidget::canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const
{
  Q_D(const qMRMLMarkupsShapeWidget);

  vtkMRMLMarkupsShapeNode* shapeNode= vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
  if (!shapeNode)
    {
    return false;
    }

  return true;
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::setMRMLMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  Q_D(qMRMLMarkupsShapeWidget);

  d->MarkupsShapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
  this->setEnabled(markupsNode != nullptr);
  if (d->MarkupsShapeNode)
  {
    d->shapeNameComboBox->setCurrentIndex( d->MarkupsShapeNode->GetShapeName());
  }
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onShapeChanged(int shapeName)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (d->MarkupsShapeNode == nullptr)
  {
    return;
  }
  d->MarkupsShapeNode->SetShapeName(shapeName);
  
  d->radiusModeLabel->setVisible(shapeName != vtkMRMLMarkupsShapeNode::Disk);
  d->radiusModeComboBox->setVisible(shapeName != vtkMRMLMarkupsShapeNode::Disk);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onRadiusModeChanged()
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetRadiusMode(d->radiusModeComboBox->currentIndex());
  d->MarkupsShapeNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onDrawModeChanged()
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetDrawMode2D(d->drawModeComboBox->currentIndex());
  d->MarkupsShapeNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onResolutionChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetResolution(value);
  d->MarkupsShapeNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onResliceNodeChanged(vtkMRMLNode * node)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetResliceNode(node);
  d->MarkupsShapeNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onResliceButtonClicked()
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->ResliceToControlPoints();
  d->MarkupsShapeNode->UpdateScene(this->mrmlScene());
}
