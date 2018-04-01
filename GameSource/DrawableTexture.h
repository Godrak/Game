#pragma once

#include "UrhoIncludeAll.h"

using namespace Urho3D;

class Curve : public LogicComponent {
URHO3D_OBJECT(Curve, LogicComponent);

    explicit Curve(Context *context) : LogicComponent(context) {}

    void AddPoint(const Vector3 &point) {
        if (points.Empty()) {
            points.Push(point);
            lineNodes.Push(CreateLine(point, point));
        } else {
            lineNodes.Push(CreateLine(points.Back(), point));
            points.Push(point);
        }

    };

    Vector<Vector3> GetPoints() {
        return points;
    }


private:
    Vector<Vector3> points{};
    Vector<Node *> lineNodes{};

    Vector<Node *> CreateLine(const Vector3 &start, const Vector3 &end) {
        Vector3 line = end - start;
        float length = line.Length();
        float drawPosition = 0;
        Vector<Node *> lineNodes{};
        while (drawPosition < length) {
            lineNodes.Push(CreateFire(start + drawPosition * line.Normalized()));
            drawPosition += DRAWING_SAMPLE_STEP_SIZE;
        }
        return lineNodes;
    }

    Node *CreateFire(Vector3 position, Vector3 scale = Vector3::ONE) {
        auto *fireNode = GetScene()->CreateChild("Fire");
        fireNode->SetPosition(position + Vector3::UP * 0.1f);
        fireNode->SetScale(scale);

        auto particleEmitter = fireNode->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/BlueLine.xml");
        particleEmitter->SetEffect(effect);
        particleEmitter->SetNumParticles(5);

        return fireNode;
    }
};


class DrawableTexture : public LogicComponent {
URHO3D_OBJECT(DrawableTexture, LogicComponent);
public:
    explicit DrawableTexture(Context *context) : LogicComponent(context) {}

    Curve *StartCurve(Vector3 start) {
        Node *curveNode = GetNode()->CreateChild();
        Curve *curve = curveNode->CreateComponent<Curve>();
        curve->AddPoint(start);
        curves.Push(curve);
        return curve;
    };


    Vector<Curve *> GetCurves() {
        return curves;
    };

private:
    Vector<Curve *> curves;

};
