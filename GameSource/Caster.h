#pragma once

#include "UrhoIncludeAll.h"
#include <iostream>
#include "DrawableTexture.h"
#include "SpellSystem.h"

using namespace Urho3D;

class Caster : public LogicComponent {
URHO3D_OBJECT(Caster, LogicComponent);
private:
    float rectangleSize = 40;
    Node *spellRectangle = NULL;
    Curve *currentCurve = NULL;
    float timeToNextSample = DRAWING_SAMPLE_TIME;
public:
    explicit Caster(Context *context) : LogicComponent(context) {};

    void draw(float timeStep) {
        if (timeToNextSample <= 0) {
            timeToNextSample = DRAWING_SAMPLE_TIME;
            Vector3 hitPos;
            DrawableTexture *drawableTexture = RaycastDrawableHit(hitPos);
            if (drawableTexture != NULL) {
                if (currentCurve != NULL) {
                    currentCurve->AddPoint(hitPos);
                } else {
                    currentCurve = drawableTexture->StartCurve(hitPos);
                }
            }
        } else {
            timeToNextSample -= timeStep;
        }
    }

    void endCurve() {
        currentCurve = NULL;
        timeToNextSample = 0;
    }

    void moveRectangle(Vector3 position) {
        if (spellRectangle == NULL) {
            createRectangle();
        }
        spellRectangle->SetScale(rectangleSize);

        spellRectangle->SetWorldPosition(position + Vector3::UP * 0.1);

    }

    void cast(DrawableTexture *drawableTexture, Vector3 position) {
        float top = spellRectangle->GetWorldPosition().z_ - spellRectangle->GetWorldScale().z_ / 2;
        float bottom = spellRectangle->GetWorldPosition().z_ + spellRectangle->GetWorldScale().z_ / 2;
        float left = spellRectangle->GetWorldPosition().x_ - spellRectangle->GetWorldScale().x_ / 2;
        float right = spellRectangle->GetWorldPosition().x_ + spellRectangle->GetWorldScale().x_ / 2;

        Vector<Curve *> curves = drawableTexture->GetCurves();
        vector<Line> lines;
        for (int i = 0; i < curves.Size(); ++i) {
            bool used = false;
            for (int j = 0; j < curves[i]->GetPoints().Size() - 1; ++j) {
                auto start = curves[i]->GetPoints()[j];
                auto end = curves[i]->GetPoints()[j + 1];
                if (checkLineBounds(start, end, top, bottom, left, right)) {
                    used = true;
                    lines.push_back(Line{float2{start.x_, -start.z_}, float2{end.x_, -end.z_}});
                }
            }
            if (used && REMOVE_USED_CURVES) {
                drawableTexture->RemoveCurve(curves[i]);
            }
        }

        CastSpell(lines, position);
    }

    void CastSpell(vector<Line> lines, Vector3 position) {
        if (lines.empty()) return;

        auto *spellNode = GetScene()->CreateTemporaryChild();
        auto *spell = spellNode->CreateComponent<Spell>();
        spell->Start(lines, position);
    }

    DrawableTexture *RaycastDrawableHit(Vector3 &hitPos) {
        float maxDistance = 100;
        UI *ui = GetSubsystem<UI>();
        IntVector2 pos = ui->GetCursorPosition();
        // Check the cursor is visible and there is no UI element in front of the cursor
        if (ui->GetElementAt(pos, true))
            return NULL;

        DrawableTexture *drawableTexture = NULL;

        auto *graphics = GetSubsystem<Graphics>();
        auto *camera = GetScene()->GetChild("Camera")->GetComponent<Camera>();
        Ray cameraRay = camera->GetScreenRay((float) pos.x_ / graphics->GetWidth(),
                                             (float) pos.y_ / graphics->GetHeight());

        PODVector<RayQueryResult> results;
        RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_ANY);
        GetScene()->GetComponent<Octree>()->Raycast(query);
        if (results.Size()) {
            for (const auto &result : results) {
                drawableTexture = result.node_->GetComponent<DrawableTexture>();
                if (drawableTexture != NULL) {
                    hitPos = result.position_;
                    break;
                }
            }
        }

        return drawableTexture;
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        moveRectangle(GetNode()->GetPosition());
    }

private:
    bool checkLineBounds(Vector3 start, Vector3 end, float top, float bottom, float left, float right) {
        return (
                (start.x_ >= left && start.x_ <= right &&
                 start.z_ >= top && start.z_ <= bottom) ||
                (end.x_ >= left && end.x_ <= right &&
                 end.z_ >= top && end.z_ <= bottom)
        );
    }

    void createRectangle() {
        spellRectangle = GetScene()->CreateChild();
        auto *planeObject = spellRectangle->CreateComponent<StaticModel>();
//        planeObject->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Plane.mdl"));
//        planeObject->SetMaterial(GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/GoldTransparent.xml"));
        spellRectangle->SetScale(rectangleSize);
    }
};
