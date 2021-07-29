#include "vtkInteractorStyleArchitecture.h"

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>
#include <vtkExtractEdges.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

vtkStandardNewMacro(vtkInteractorStyleArchitecture);

//----------------------------------------------------------------------------
vtkInteractorStyleArchitecture::vtkInteractorStyleArchitecture()
{
    this->LatLongLines = 0;

    this->LatLongSphere = nullptr;
    this->LatLongExtractEdges = nullptr;
    this->LatLongMapper = nullptr;
    this->LatLongActor = nullptr;

    this->MotionFactor = 10.0;
}

//----------------------------------------------------------------------------
vtkInteractorStyleArchitecture::~vtkInteractorStyleArchitecture()
{
    if (this->LatLongSphere != nullptr)
    {
        this->LatLongSphere->Delete();
    }

    if (this->LatLongMapper != nullptr)
    {
        this->LatLongMapper->Delete();
    }

    if (this->LatLongActor != nullptr)
    {
        this->LatLongActor->Delete();
    }

    if (this->LatLongExtractEdges != nullptr)
    {
        this->LatLongExtractEdges->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnMouseMove()
{
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    switch (this->State)
    {
    case VTKIS_ROTATE:
        this->FindPokedRenderer(x, y);
        this->Rotate();
        this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
        break;

    case VTKIS_PAN:
        this->FindPokedRenderer(x, y);
        this->Pan();
        this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
        break;

    case VTKIS_DOLLY:
        this->FindPokedRenderer(x, y);
        this->Dolly();
        this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
        break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnLeftButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    this->StartRotate();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnLeftButtonUp()
{
    switch (this->State)
    {
    case VTKIS_ROTATE:
        this->EndRotate();
        if (this->Interactor)
        {
            this->ReleaseFocus();
        }
        break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnMiddleButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    this->StartPan();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnMiddleButtonUp()
{
    switch (this->State)
    {
    case VTKIS_PAN:
        this->EndPan();
        if (this->Interactor)
        {
            this->ReleaseFocus();
        }
        break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnRightButtonDown()
{
    this->OnMiddleButtonDown();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnRightButtonUp()
{
    this->OnMiddleButtonUp();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnMouseWheelForward()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    this->StartDolly();
    double factor = this->MotionFactor * 0.2 * this->MouseWheelMotionFactor;
    this->Dolly(pow(1.1, factor));
    this->EndDolly();
    this->ReleaseFocus();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnMouseWheelBackward()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    this->StartDolly();
    double factor = this->MotionFactor * -0.2 * this->MouseWheelMotionFactor;
    this->Dolly(pow(1.1, factor));
    this->EndDolly();
    this->ReleaseFocus();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::Rotate()
{
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    vtkRenderWindowInteractor* rwi = this->Interactor;

    int dx = -(rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0]);
    int dy = -(rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1]);

    int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

    double a = dx / static_cast<double>(size[0]) * 180.0;
    double e = dy / static_cast<double>(size[1]) * 180.0;

    if (rwi->GetShiftKey())
    {
        if (abs(dx) >= abs(dy))
        {
            e = 0.0;
        }
        else
        {
            a = 0.0;
        }
    }

    // Move the camera.
    // Make sure that we don't hit the north pole singularity.

    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    camera->Azimuth(a * 4);

    double dop[3], vup[3];

    camera->GetDirectionOfProjection(dop);
    vtkMath::Normalize(dop);
    camera->GetViewUp(vup);
    vtkMath::Normalize(vup);

    double angle = vtkMath::DegreesFromRadians(acos(vtkMath::Dot(dop, vup)));
    if ((angle + e) > 179.0 ||
        (angle + e) < 1.0)
    {
        e = 0.0;
    }

    camera->Elevation(e);

    if (this->AutoAdjustCameraClippingRange)
    {
        this->CurrentRenderer->ResetCameraClippingRange();
    }

    rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::Pan()
{
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    vtkRenderWindowInteractor* rwi = this->Interactor;

    // Get the vector of motion

    double fp[3], focalPoint[3], pos[3], v[3], p1[4], p2[4];

    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    camera->GetPosition(pos);
    camera->GetFocalPoint(fp);

    this->ComputeWorldToDisplay(fp[0], fp[1], fp[2],
                                focalPoint);

    this->ComputeDisplayToWorld(rwi->GetEventPosition()[0],
                                rwi->GetEventPosition()[1],
                                focalPoint[2],
                                p1);

    this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0],
                                rwi->GetLastEventPosition()[1],
                                focalPoint[2],
                                p2);

    for (int i = 0; i < 3; i++)
    {
        v[i] = p2[i] - p1[i];
        pos[i] += v[i];
        fp[i] += v[i];
    }

    camera->SetPosition(pos);
    camera->SetFocalPoint(fp);

    if (rwi->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::Dolly()
{
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    vtkRenderWindowInteractor* rwi = this->Interactor;
    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    double* center = this->CurrentRenderer->GetCenter();

    int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
    double dyf = this->MotionFactor * dy / center[1];
    double zoomFactor = pow(1.1, dyf);

    if (camera->GetParallelProjection())
    {
        camera->SetParallelScale(camera->GetParallelScale() / zoomFactor);
    }
    else
    {
        camera->Dolly(zoomFactor);
        if (this->AutoAdjustCameraClippingRange)
        {
            this->CurrentRenderer->ResetCameraClippingRange();
        }
    }

    if (rwi->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::OnChar()
{
    vtkRenderWindowInteractor* rwi = this->Interactor;

    switch (rwi->GetKeyCode())
    {
    case 'l':
        this->FindPokedRenderer(rwi->GetEventPosition()[0],
                                rwi->GetEventPosition()[1]);
        this->CreateLatLong();
        if (this->LatLongLines)
        {
            this->LatLongLinesOff();
        }
        else
        {
            double bounds[6];
            this->CurrentRenderer->ComputeVisiblePropBounds(bounds);
            double radius = sqrt((bounds[1] - bounds[0]) * (bounds[1] - bounds[0]) +
                                 (bounds[3] - bounds[2]) * (bounds[3] - bounds[2]) +
                                 (bounds[5] - bounds[4]) * (bounds[5] - bounds[4])) / 2.0;
            this->LatLongSphere->SetRadius(radius);
            this->LatLongSphere->SetCenter((bounds[0] + bounds[1]) / 2.0,
                                           (bounds[2] + bounds[3]) / 2.0,
                                           (bounds[4] + bounds[5]) / 2.0);
            this->LatLongLinesOn();
        }
        this->SelectRepresentation();
        rwi->Render();
        break;

    default:
        this->Superclass::OnChar();
        break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::CreateLatLong()
{
    if (this->LatLongSphere == nullptr)
    {
        this->LatLongSphere = vtkSphereSource::New();
        this->LatLongSphere->SetPhiResolution(13);
        this->LatLongSphere->SetThetaResolution(25);
        this->LatLongSphere->LatLongTessellationOn();
    }

    if (this->LatLongExtractEdges == nullptr)
    {
        this->LatLongExtractEdges = vtkExtractEdges::New();
        this->LatLongExtractEdges->SetInputConnection(
            this->LatLongSphere->GetOutputPort());
    }

    if (this->LatLongMapper == nullptr)
    {
        this->LatLongMapper = vtkPolyDataMapper::New();
        this->LatLongMapper->SetInputConnection(
            this->LatLongExtractEdges->GetOutputPort());
    }

    if (this->LatLongActor == nullptr)
    {
        this->LatLongActor = vtkActor::New();
        this->LatLongActor->SetMapper(this->LatLongMapper);
        this->LatLongActor->PickableOff();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::SelectRepresentation()
{
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    this->CurrentRenderer->RemoveActor(this->LatLongActor);

    if (this->LatLongLines)
    {
        this->CurrentRenderer->AddActor(this->LatLongActor);
        this->LatLongActor->VisibilityOn();
    }
    else
    {
        this->LatLongActor->VisibilityOff();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::Dolly(double factor)
{
    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    if (camera->GetParallelProjection())
    {
        camera->SetParallelScale(camera->GetParallelScale() / factor);
    }
    else
    {
        camera->Dolly(factor);
        if (this->AutoAdjustCameraClippingRange)
        {
            this->CurrentRenderer->ResetCameraClippingRange();
        }
    }

    if (this->Interactor->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleArchitecture::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Latitude/Longitude Lines: "
        << (this->LatLongLines ? "On\n" : "Off\n");
}
