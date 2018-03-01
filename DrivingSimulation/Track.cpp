#include "stdafx.h"

#include <DirectXMath.h>

#include "Track.h"
#include "Geometry.h"

Track::Track() :
    m_laneWidth(0.0f){}
Track::Track(const Track& other){}
Track::~Track(){}

bool Track::Initialize(ID3D11Device* device,
                       ID3D11DeviceContext* context,
                       const float laneWidth,
                       const char* trackFilename,
                       const char* textureFilename,
                       const Texture::TextureImageFormat imageFormat)
{
    bool success = false;
    
    m_laneWidth = laneWidth;
    m_trackFilename = trackFilename;

    if(LoadTexture(device, context, textureFilename, imageFormat))
    {
        success = InitializeBuffers(device);
    }

    return success;
}

bool Track::Initialize(ID3D11Device* device,
                       const float laneWidth,
                       const char* trackFilename,
                       const DirectX::XMFLOAT4 color)
{
    m_laneWidth = laneWidth;
    m_trackFilename = trackFilename;
    m_color = color;

    return InitializeBuffers(device);
}

void Track::Shutdown()
{
    m_laneWidth = 0.0f;
    m_centerPoints.clear();

    Model::Shutdown();
}

void Track::GenerateCurve(const DirectX::XMFLOAT2 centerPoint,
                          const float radius,
                          const float startAngle,
                          const float endAngle,
                          const float angularStepAmount,
                          const bool clockwise)
{
    DirectX::XMFLOAT2 lastPoint(centerPoint.x + (radius * cosf(endAngle)),
                               centerPoint.y + (radius * sinf(endAngle)));

    if(clockwise)
    {
        float angle = startAngle < endAngle ? startAngle + DirectX::XM_2PI : startAngle;

        while(angle > endAngle)
        {
            DirectX::XMFLOAT2 point(centerPoint.x + (radius * cosf(angle)),
                                    centerPoint.y + (radius * sinf(angle)));

            //if point is indistinguishable from last point, stop creating intermediate points
            if(!Geometry::AlmostEqual(Geometry::Distance(point, lastPoint), 0.0f))
            {
                m_centerPoints.push_back(point);
            }
            else
            {
                break;
            }

            //another check to ensure last intermediate point isn't exceptionally short
            if(angle - (2 * angularStepAmount) < endAngle)
            {
                break;
            }

            angle -= angularStepAmount;
        }
    }
    else
    {
        float angle = startAngle > endAngle ? startAngle - DirectX::XM_2PI : startAngle;

        while(angle < endAngle)
        {
            DirectX::XMFLOAT2 point(centerPoint.x + (radius * cosf(angle)),
                                    centerPoint.y + (radius * sinf(angle)));

            //if point is indistinguishable from last point, stop creating intermediate points
            if(!Geometry::AlmostEqual(Geometry::Distance(point, lastPoint), 0.0f))
            {
                m_centerPoints.push_back(point);
            }
            else
            {
                break;
            }

            //another check to ensure last intermediate point isn't exceptionally short
            if(angle + 2 * angularStepAmount > endAngle)
            {
                break;
            }

            angle += angularStepAmount;
        }
    }

    m_centerPoints.push_back(lastPoint);
}

bool Track::InitializeBuffers(ID3D11Device* device)
{
    bool success = true;

    m_centerPoints.push_back(DirectX::XMFLOAT2(0.0f, 0.0f));
    Track::GenerateCurve(DirectX::XMFLOAT2(200.0f, 75.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         false);
    
    Track::GenerateCurve(DirectX::XMFLOAT2(75.0f, 225.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(275.0f, 225.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(0.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(275.0f, 25.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(0.0f),
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(100.0f, -125.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         false);

    Track::GenerateCurve(DirectX::XMFLOAT2(350.0f, -275.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(-300.0f, -275.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(180.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(-300.0f, -225.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(180.0f),
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(-250.0f, -75.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(0.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         false);

    Track::GenerateCurve(DirectX::XMFLOAT2(-250.0f, -25.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(0.0f),
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         false);

    Track::GenerateCurve(DirectX::XMFLOAT2(-375.0f, 125.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(180.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(-375.0f, 375.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(180.0f),
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(-200.0f, 375.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(90.0f),
                         DirectX::XMConvertToRadians(0.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         true);

    Track::GenerateCurve(DirectX::XMFLOAT2(-50.0f, 75.0f),
                         75.0f,
                         DirectX::XMConvertToRadians(180.0f),
                         DirectX::XMConvertToRadians(270.0f),
                         DirectX::XMConvertToRadians(2.5f),
                         false);

    m_centerPoints.push_back(DirectX::XMFLOAT2(0.0f, 0.0f));
    
    if(m_centerPoints.size() > 1)
    {   
        std::vector<DirectX::XMFLOAT3> vertices;
        std::vector<TextureVertex> joints;

        DirectX::XMFLOAT3 commonFilletPoint;
        bool lastSegmentRequiredFillet = false;
        bool wasLeftFillet = false;

        for(unsigned int i = 0; i < m_centerPoints.size() - 1; ++i)
        {
            DirectX::XMFLOAT2 startPoint = m_centerPoints[i];
            DirectX::XMFLOAT2 endPoint = m_centerPoints[i + 1];

            float currentSegmentAngle = Geometry::GetVectorAngle(startPoint, endPoint);
            float nextSegmentAngle = currentSegmentAngle;

            //TODO: exceptionally long segments seem to have blurry textures when viewed from afar. 
            //Perhaps try breaking these segments into lanewidth x lanewidth chunks.

            if(i + 2 < m_centerPoints.size())
            {
                nextSegmentAngle = Geometry::GetVectorAngle(endPoint, m_centerPoints[i + 2]);
            }

            float angularDiff = nextSegmentAngle - currentSegmentAngle;
            
            if(angularDiff < 0.0f)
            {
                angularDiff += DirectX::XM_2PI;
            }

            float height = 0.02f;

            DirectX::XMFLOAT3 rearLeftCorner;

            if(lastSegmentRequiredFillet && wasLeftFillet)
            {
                rearLeftCorner = commonFilletPoint;
                lastSegmentRequiredFillet = false;
            }
            else
            {
                rearLeftCorner.x = startPoint.x + (m_laneWidth * cosf(currentSegmentAngle + DirectX::XM_PIDIV2));
                rearLeftCorner.y = height;
                rearLeftCorner.z = startPoint.y + (m_laneWidth * sinf(currentSegmentAngle + DirectX::XM_PIDIV2));
            }

            DirectX::XMFLOAT3 rearRightCorner;

            if(lastSegmentRequiredFillet && !wasLeftFillet)
            {
                rearRightCorner = commonFilletPoint;
                lastSegmentRequiredFillet = false;
            }
            else
            {
                rearRightCorner.x = startPoint.x + (m_laneWidth * cosf(currentSegmentAngle - DirectX::XM_PIDIV2));
                rearRightCorner.y = height;
                rearRightCorner.z = startPoint.y + (m_laneWidth * sinf(currentSegmentAngle - DirectX::XM_PIDIV2));
            }

            DirectX::XMFLOAT3 frontLeftCorner;
            frontLeftCorner.x = endPoint.x + (m_laneWidth * cosf(currentSegmentAngle + DirectX::XM_PIDIV2));
            frontLeftCorner.y = height;
            frontLeftCorner.z = endPoint.y + (m_laneWidth * sinf(currentSegmentAngle + DirectX::XM_PIDIV2));
            
            DirectX::XMFLOAT3 frontRightCorner;
            frontRightCorner.x = endPoint.x + (m_laneWidth * cosf(currentSegmentAngle - DirectX::XM_PIDIV2));
            frontRightCorner.y = height;
            frontRightCorner.z = endPoint.y + (m_laneWidth * sinf(currentSegmentAngle - DirectX::XM_PIDIV2));

            if(!Geometry::AlmostEqual(angularDiff, 0.0f) && i + 2 < m_centerPoints.size())
            {
                if(angularDiff <= DirectX::XM_PI)
                {
                    DirectX::XMFLOAT2 nextRearLeft;
                    nextRearLeft.x = endPoint.x + (m_laneWidth * cosf(nextSegmentAngle + DirectX::XM_PIDIV2));
                    nextRearLeft.y = endPoint.y + (m_laneWidth * sinf(nextSegmentAngle + DirectX::XM_PIDIV2));

                    DirectX::XMFLOAT2 nextFrontLeft;
                    nextFrontLeft.x = m_centerPoints[i + 2].x + (m_laneWidth * cosf(nextSegmentAngle + DirectX::XM_PIDIV2));
                    nextFrontLeft.y = m_centerPoints[i + 2].y + (m_laneWidth * sinf(nextSegmentAngle + DirectX::XM_PIDIV2));

                    DirectX::XMFLOAT2 intersection;

                    bool hasIntersection = Geometry::FindIntersectionBetweenLines(DirectX::XMFLOAT2(rearLeftCorner.x,
                                                                                                    rearLeftCorner.z),
                                                                                  DirectX::XMFLOAT2(frontLeftCorner.x,
                                                                                                    frontLeftCorner.z),
                                                                                  nextRearLeft,
                                                                                  nextFrontLeft,
                                                                                  intersection);
                    
                    if(hasIntersection)
                    {
                        float diffX = intersection.x - frontLeftCorner.x;
                        float diffZ = intersection.y - frontLeftCorner.z;

                        //shorten current segment
                        frontLeftCorner.x += diffX;
                        frontLeftCorner.z += diffZ;
                        frontRightCorner.x += diffX;
                        frontRightCorner.z += diffZ;

                        //shorten next segment preemtively
                        float segmentReduction = hypotf(diffX, diffZ);

                        DirectX::XMFLOAT2 nextStart = endPoint;
                        nextStart.x = m_centerPoints[i + 1].x + (segmentReduction * cosf(nextSegmentAngle));
                        nextStart.y = m_centerPoints[i + 1].y + (segmentReduction * sinf(nextSegmentAngle));
                        m_centerPoints[i + 1] = nextStart;

                        //fill the joint between this road segment and the next
                        DirectX::XMFLOAT3 normal(0.0f, 1.0f, 0.0f);

                        TextureVertex jointVertex1;
                        jointVertex1.position = DirectX::XMFLOAT3(frontLeftCorner.x,
                                                                  frontLeftCorner.y,
                                                                  frontLeftCorner.z);
                        jointVertex1.texture = DirectX::XMFLOAT2(0.5f, 0.0f);
                        jointVertex1.normal = normal;
                        joints.push_back(jointVertex1);

                        TextureVertex jointVertex2;
                        jointVertex2.position = DirectX::XMFLOAT3(nextStart.x + (m_laneWidth * cosf(nextSegmentAngle - DirectX::XM_PIDIV2)),
                                                                  height,
                                                                  nextStart.y + (m_laneWidth * sinf(nextSegmentAngle - DirectX::XM_PIDIV2)));
                        jointVertex2.texture = DirectX::XMFLOAT2(0.5f + (sinf(angularDiff) / 2), 1.0f);
                        jointVertex2.normal = normal;
                        joints.push_back(jointVertex2);

                        TextureVertex jointVertex3;
                        jointVertex3.position = DirectX::XMFLOAT3(frontRightCorner.x,
                                                                  frontRightCorner.y,
                                                                  frontRightCorner.z);
                        jointVertex3.texture = DirectX::XMFLOAT2(0.5f - (sinf(angularDiff) / 2), 1.0f);
                        jointVertex3.normal = normal;
                        joints.push_back(jointVertex3);

                        commonFilletPoint = frontLeftCorner;
                        lastSegmentRequiredFillet = true;
                        wasLeftFillet = true;
                    }
                }
                else
                {
                    DirectX::XMFLOAT2 nextRearRight;
                    nextRearRight.x = endPoint.x + (m_laneWidth * cosf(nextSegmentAngle - DirectX::XM_PIDIV2));
                    nextRearRight.y = endPoint.y + (m_laneWidth * sinf(nextSegmentAngle - DirectX::XM_PIDIV2));

                    DirectX::XMFLOAT2 nextFrontRight;
                    nextFrontRight.x = m_centerPoints[i + 2].x + (m_laneWidth * cosf(nextSegmentAngle - DirectX::XM_PIDIV2));
                    nextFrontRight.y = m_centerPoints[i + 2].y + (m_laneWidth * sinf(nextSegmentAngle - DirectX::XM_PIDIV2));

                    DirectX::XMFLOAT2 intersection;

                    bool hasIntersection = Geometry::FindIntersectionBetweenLines(DirectX::XMFLOAT2(rearRightCorner.x, rearRightCorner.z),
                                                                                  DirectX::XMFLOAT2(frontRightCorner.x, frontRightCorner.z),
                                                                                  nextRearRight,
                                                                                  nextFrontRight,
                                                                                  intersection);

                    if(hasIntersection)
                    {
                        float diffX = intersection.x - frontRightCorner.x;
                        float diffZ = intersection.y - frontRightCorner.z;

                        //shorten current segment
                        frontLeftCorner.x += diffX;
                        frontLeftCorner.z += diffZ;
                        frontRightCorner.x += diffX;
                        frontRightCorner.z += diffZ;

                        //shorten next segment preemtively
                        float segmentReduction = hypotf(diffX, diffZ);

                        DirectX::XMFLOAT2 nextStart = endPoint;
                        nextStart.x = m_centerPoints[i + 1].x + (segmentReduction * cosf(nextSegmentAngle));
                        nextStart.y = m_centerPoints[i + 1].y + (segmentReduction * sinf(nextSegmentAngle));
                        m_centerPoints[i + 1] = nextStart;

                        //fill the joint between this road segment and the next
                        DirectX::XMFLOAT3 normal(0.0f, 1.0f, 0.0f);

                        TextureVertex jointVertex1;
                        jointVertex1.position = DirectX::XMFLOAT3(frontRightCorner.x,
                                                                  frontRightCorner.y,
                                                                  frontRightCorner.z);
                        jointVertex1.texture = DirectX::XMFLOAT2(0.5f, 1.0f);
                        jointVertex1.normal = normal;
                        joints.push_back(jointVertex1);

                        TextureVertex jointVertex2;
                        jointVertex2.position = DirectX::XMFLOAT3(frontLeftCorner.x,
                                                                  frontLeftCorner.y,
                                                                  frontLeftCorner.z);
                        jointVertex2.texture = DirectX::XMFLOAT2(0.5f - (sinf(angularDiff) / 2), 0.0f);
                        jointVertex2.normal = normal;
                        joints.push_back(jointVertex2);

                        TextureVertex jointVertex3;
                        jointVertex3.position = DirectX::XMFLOAT3(nextStart.x + (m_laneWidth * cosf(nextSegmentAngle + DirectX::XM_PIDIV2)),
                                                                  height,
                                                                  nextStart.y + (m_laneWidth * sinf(nextSegmentAngle + DirectX::XM_PIDIV2)));
                        jointVertex3.texture = DirectX::XMFLOAT2(0.5f + (sinf(angularDiff) / 2), 0.0f);
                        jointVertex3.normal = normal;
                        joints.push_back(jointVertex3);

                        commonFilletPoint = frontRightCorner;
                        lastSegmentRequiredFillet = true;
                        wasLeftFillet = false;
                    }
                }
            }

            //poly 1
            vertices.push_back(rearLeftCorner);
            vertices.push_back(frontLeftCorner);
            vertices.push_back(rearRightCorner);

            //poly 2
            vertices.push_back(rearRightCorner);
            vertices.push_back(frontLeftCorner);
            vertices.push_back(frontRightCorner);
        }

        m_vertexCount = vertices.size() + joints.size();
        m_indexCount = m_vertexCount;
        m_indices = new unsigned int[m_indexCount];

        unsigned int index = 0;

        if(m_texture)
        {
            m_vertices = new TextureVertex[m_vertexCount];
            
            for(unsigned int i = 0; i < joints.size(); ++i)
            {
                static_cast<TextureVertex*>(m_vertices)[index].position = joints[i].position;
                static_cast<TextureVertex*>(m_vertices)[index].texture = joints[i].texture;
                static_cast<TextureVertex*>(m_vertices)[index].normal = joints[i].normal;
                m_indices[i] = i;
                ++index;
            }
        }
        else
        {
            m_vertices = new ColorVertex[m_vertexCount];

            for(unsigned int i = 0; i < joints.size(); ++i)
            {
                static_cast<ColorVertex*>(m_vertices)[index].position = joints[i].position;
                static_cast<ColorVertex*>(m_vertices)[index].color = m_color;
                m_indices[i] = i;
                ++index;
            }
        }

        if(m_texture)
        {
            for(unsigned int i = 0; i < vertices.size(); i +=6)
            {
                float segmentLength = hypotf(abs(vertices[i].x - vertices[i + 1].x),
                                             abs(vertices[i].z - vertices[i + 1].z));
                float textureRatio = segmentLength / (2 * m_laneWidth);

                //Triangle 1 - Rear left
                static_cast<TextureVertex*>(m_vertices)[index].position = vertices[i];
                static_cast<TextureVertex*>(m_vertices)[index].texture.x = textureRatio > 1.0f ? 0.0f : 0.5f - textureRatio / 2;
                static_cast<TextureVertex*>(m_vertices)[index].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.z = 0.0f;
                m_indices[index] = index;
                ++index;

                //Triangle 1 - Front left
                static_cast<TextureVertex*>(m_vertices)[index].position = vertices[i + 1];
                static_cast<TextureVertex*>(m_vertices)[index].texture.x = textureRatio > 1.0f ? textureRatio : 0.5f + (textureRatio / 2);
                static_cast<TextureVertex*>(m_vertices)[index].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.z = 0.0f;
                m_indices[index] = index;
                ++index;

                //Triangle 1 - Rear right
                static_cast<TextureVertex*>(m_vertices)[index].position = vertices[i + 2];
                static_cast<TextureVertex*>(m_vertices)[index].texture.x = textureRatio > 1.0f ? 0.0f : 0.5f - textureRatio / 2;
                static_cast<TextureVertex*>(m_vertices)[index].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.z = 0.0f;
                m_indices[index] = index;
                ++index;

                //Triangle 2 - Rear right
                static_cast<TextureVertex*>(m_vertices)[index].position = vertices[i + 3];
                static_cast<TextureVertex*>(m_vertices)[index].texture.x = textureRatio > 1.0f ? 0.0f : 0.5f - textureRatio / 2;
                static_cast<TextureVertex*>(m_vertices)[index].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.z = 0.0f;
                m_indices[index] = index;
                ++index;

                //Triangle 2 - Front left
                static_cast<TextureVertex*>(m_vertices)[index].position = vertices[i + 4];
                static_cast<TextureVertex*>(m_vertices)[index].texture.x = textureRatio > 1.0f ? textureRatio : 0.5f + (textureRatio / 2);
                static_cast<TextureVertex*>(m_vertices)[index].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.z = 0.0f;
                m_indices[index] = index;
                ++index;

                //Triangle 2 - Front right
                static_cast<TextureVertex*>(m_vertices)[index].position = vertices[i + 5];
                static_cast<TextureVertex*>(m_vertices)[index].texture.x = textureRatio > 1.0f ? textureRatio : 0.5f + (textureRatio / 2);
                static_cast<TextureVertex*>(m_vertices)[index].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[index].normal.z = 0.0f;
                m_indices[index] = index;
                ++index;
            }
        }
        else
        {
            for(unsigned int i = 0; i < vertices.size(); i += 6)
            {
                unsigned int index = i;

                //Triangle 1 - Rear left
                static_cast<ColorVertex*>(m_vertices)[index].position = vertices[i];
                static_cast<ColorVertex*>(m_vertices)[index].color = m_color;
                m_indices[index] = index;
                ++index;

                //Triangle 1 - Rear right
                static_cast<ColorVertex*>(m_vertices)[index].position = vertices[i + 1];
                static_cast<ColorVertex*>(m_vertices)[index].color = m_color;
                m_indices[index] = index;
                ++index;

                //Triangle 1 - Front left
                static_cast<ColorVertex*>(m_vertices)[index].position = vertices[i + 2];
                static_cast<ColorVertex*>(m_vertices)[index].color = m_color;
                m_indices[index] = index;
                ++index;

                //Triangle 2 - Front left
                static_cast<ColorVertex*>(m_vertices)[index].position = vertices[i + 3];
                static_cast<ColorVertex*>(m_vertices)[index].color = m_color;
                m_indices[index] = index;
                ++index;

                //Triangle 2 - Rear right
                static_cast<ColorVertex*>(m_vertices)[index].position = vertices[i + 4];
                static_cast<ColorVertex*>(m_vertices)[index].color = m_color;
                m_indices[index] = index;
                ++index;

                //Triangle 2 - Front right
                static_cast<ColorVertex*>(m_vertices)[index].position = vertices[i + 5];
                static_cast<ColorVertex*>(m_vertices)[index].color = m_color;
                m_indices[index] = index;
                ++index;
            }
        }
    }
    else
    {
        success = false;
    }

    return success && Model::InitializeBuffers(device);
}