//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include <algorithm>

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h, ColliderLayer layer, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    mOwner->GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    mOwner->GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1.1 (1 linha): Calcule (e retorne) o ponto mínimo dessa AABB. A variável `mOffset`
    //  define a posição da AABB com relação a posição do objeto dono do componente. Portanto,
    //  basta somar a posição do objeto dono do componente a esse deslocamento.
    return this->mOffset + this->mOwner->GetPosition();



    return {};
}

Vector2 AABBColliderComponent::GetMax() const
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1.2 (1 linha): Calcule (e retorne) o ponto máximo dessa AABB. As variáveis membro
    //  `mWidth` e `mHeight` definem a altura e a largura da AABB, respectivamente. Portanto,
    //  basta somar a largura à coordenada x e a altura à coordenada y do ponto mínimo da AABB
    //  (utilize o método `GetMin` implementado na etapa anterior).
    return Vector2(this->mWidth + this->GetMin().x, this->mHeight + this->GetMin().y);



    return {};
}

Vector2 AABBColliderComponent::GetCenter() const
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1.3 (1 linha): Calcule (e retorne) o centro dessa AABB. Esse ponto pode ser calculado
    //  de forma similar ao ponto máximo, basta somar a metade da largura à coordenada x e a metade da altura
    //  à coordenada y do ponto mínimo da AABB (utilize o método `GetMin` implementado anteriormente).


    return Vector2(this->mWidth/2 + this->GetMin().x, this->mHeight/2 + this->GetMin().y);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 2.1 (~5 linhas): Verifique se esta AABB está colidindo com a AABB b passada como parâmetro.
    //  Retorne verdadeiro se estiver e falso caso contrário. Utilize os métodos `GetMin` e `GetMax`
    //  para acessar os pontos de mínimo e máximo das duas AABBs.
    // Calculate the min and max values for this AABB
    Vector2 thisMin = GetMin();
    Vector2 thisMax = GetMax();

    // Calculate the min and max values for the AABB b
    Vector2 bMin = b.GetMin();
    Vector2 bMax = b.GetMax();

    // Check for intersection
    if (thisMin.x <= bMax.x && thisMax.x >= bMin.x &&
        thisMin.y <= bMax.y && thisMax.y >= bMin.y) {
        return true; // AABBs are intersecting
    }

    return false;
}

AABBColliderComponent::Overlap AABBColliderComponent::GetMinOverlap(AABBColliderComponent* b) const
{
    // --------------
    // TODO - PARTE 2
    // --------------
    std::unordered_map<int, Overlap> overlaps;

    // TODO 3.1 (~4 linhas): Armazene no mapa `overlaps` as sobreposições (com sinal -/+) dos quatro lados da
    //  colisão: esquerda, direita, cima e baixo. Utilize os métodos `GetMin` e `GetMax` para acessar os
    //  pontos de mínimo e máximo das duas AABBs.
    Vector2 thisMin = GetMin();
    Vector2 thisMax = GetMax();
    Vector2 bMin = b->GetMin();
    Vector2 bMax = b->GetMax();
    Overlap left;
    Overlap rigth;
    Overlap top;
    Overlap down;

    left.amount = bMax.x - thisMin.x;
    left.side = CollisionSide::Left;
    left.target = const_cast<AABBColliderComponent*>(this);

    rigth.amount = bMin.x - thisMax.x;
    rigth.side = CollisionSide::Right;
    rigth.target = const_cast<AABBColliderComponent*>(this);

    top.amount = bMax.y - thisMin.y;
    top.side = CollisionSide::Top;
    top.target = const_cast<AABBColliderComponent*>(this);

    down.amount =  bMin.y - thisMax.y;
    down.side = CollisionSide::Down;
    down.target = const_cast<AABBColliderComponent*>(this);

    overlaps[0] = left;
    overlaps[1] = rigth;
    overlaps[2] = top;
    overlaps[3] = down;

    // TODO 3.2 (~4 linhas): Encontre e retorne a sobreposição mínima. Para isso, utilize
    //  os valores absolutos das sobreposições calculadas na etapa anterior.
    Overlap minOverlap = overlaps.begin()->second;
    for (const auto& overlap : overlaps) {
        if (std::abs(overlap.second.amount) < std::abs(minOverlap.amount)) {
            minOverlap = overlap.second;
        }
    }

    return minOverlap;
}

void AABBColliderComponent::ResolveCollisions(RigidBodyComponent *rigidBody, const Overlap& minOverlap)
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 4.1 (~4 linhas): Verifique se a sobreposição `minOverlap` ocorreu no lado de cima `CollisionSide::Top`
    //  com velocidade vertical negativa ou no lado de baixo `CollisionSide::Down` com velocidade vertical positiva.
    //  Note que a estrutura `minOverlap` já possui o lado onde ocorreu a colisão `minOverlap.side`.
    //  Se um desses dois casos for verdadeiro, some a quantidade de sobreposição `minOverlap.amount`
    //  à posição vertical do dono dessa AABB e reinicialize sua velocidade vertical para zero .
    //  Dica: para verificar, por exemplo, se a colisão foi por cima, basta comparar se minOverlap.side
    //  é igual a CollisionSide::Top.
    if (minOverlap.side == CollisionSide::Top && rigidBody->GetVelocity().y < 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x, mOwner->GetPosition().y + minOverlap.amount));
        rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0));
    } else if (minOverlap.side == CollisionSide::Down && rigidBody->GetVelocity().y > 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x, mOwner->GetPosition().y + minOverlap.amount));
        rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0));
    } else if (minOverlap.side == CollisionSide::Left && rigidBody->GetVelocity().x < 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x + minOverlap.amount, mOwner->GetPosition().y));
        rigidBody->SetVelocity(Vector2(0, rigidBody->GetVelocity().y));
    } else if (minOverlap.side == CollisionSide::Right && rigidBody->GetVelocity().x > 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x + minOverlap.amount, mOwner->GetPosition().y));
        rigidBody->SetVelocity(Vector2(0, rigidBody->GetVelocity().y));
    }

    // TODO 4.2 (~4 linhas): Caso nenhum dos dois casos anteriores sejam verdadeiros, verifique se a sobreposição
    //  `minOverlap` ocorreu no lado esquerdo `CollisionSide::Left` com velocidade horizontal negativa ou no lado direito
    //  `CollisionSide::Right` com velocidade horizontal positiva. Se um desses dois casos for verdadeiro, some a quantidade
    //  de sobreposição `minOverlap.amount` à posição horizontal do dono dessa AABB e reinicialize sua velocidade horizontal
    //  para zero.
    return;
}

void AABBColliderComponent::DetectCollision(RigidBodyComponent *rigidBody)
{
    // --------------
    // TODO - PARTE 2
    // --------------
    // Sort colliders by amount to the player (center-to-center)
    auto colliders = mOwner->GetGame()->GetColliders();

    // TODO 5.1 (~3 linhas): Utilize a função `std::sort` para ordenar o vetor `colliders` de acordo com a
    //  distância entre o centro dessa AABB e o centro de cada AABB desse vetor. O vetor `colliders` contém
    //  as AABBs de todos os atores do jogo (mario, goombas e blocos). Ordenar esse vetor dessa forma fará
    //  com que as colisões mais próximas sejam resolvidas primeiro, zerando as velocidades dos objetos na
    //  ordem esperada.
    if(this->GetLayer() != ColliderLayer::Blocks)
    std::sort(colliders.begin(), colliders.end(), [this](const AABBColliderComponent* a, const AABBColliderComponent* b) {
        float distanceA = (a->GetCenter() - this->GetCenter()).LengthSq();
        float distanceB = (b->GetCenter() - this->GetCenter()).LengthSq();
        return distanceA < distanceB;
    });
    std::unordered_map<CollisionSide, Overlap> responses;
    bool verticalCollision = false;
    bool horizontalCollision = false;

    // TODO 5.2: Utilize um laço para percorra o vetor `colliders` ordenado,
    //  verificando colisões com cada AABB alvo. Em cada iteração do laço, execute as seguintes operações.
    //  - 5.2.1 (~2 linhas): verifique se o elemento corrente é a AABB desse objeto (this). Se for, continue para a próximo
    //    elemento, pois não precisamos verificar colisão de uma AABB com ela mesma.
    //  - 5.2.2 (~2 linhas): verifique se o elemento corrente está habilitado (IsEnabled()). Se não estiver, continue para a próximo
    //    elemento, pois não queremos verificar colisão de uma AABB desabilitada.
    //  - 5.2.3 (~3 linhas): Utilize a função `GetMinOverlap` para obter a sobreposição mínima dessa AABB com o elemento corrente.
    //    Em seguida, utilize a função `ResolveCollisions` para resolver a colisão entre essa AABB e o elemento corrente. Por fim,
    //    armazene os dados dessa sobreposição no mapa `responses`. Essa mapa será utilizado para enviar uma mensagem
    //    de callback `OnCollision` para o objeto dono dessa AABB.
    //  - 5.2.4 (~3 linhas): Verifique se já houve uma colisão vertical e uma horizontal durante o laço.
    //    Se sim, interrompa o laço (break), pois não precisamos verificar mais colisões.

    for (const auto& collider : colliders) {
        // 5.2.1
        if (collider == this) {
            continue;
        }

        // 5.2.2
        if (!collider->IsEnabled()) {
            continue;
        }

        // 5.2.3
        if(Intersect(*collider)){
            auto minOverlap = GetMinOverlap(collider);
            ResolveCollisions(rigidBody, minOverlap);
            responses.insert(std::make_pair(minOverlap.side, minOverlap));

            // 5.2.4
            if (minOverlap.side == CollisionSide::Top || minOverlap.side == CollisionSide::Down) {
                verticalCollision = true;
            } else if (minOverlap.side == CollisionSide::Left || minOverlap.side == CollisionSide::Right) {
                horizontalCollision = true;
            }
        }



        // Check if both vertical and horizontal collisions occurred
        if (verticalCollision && horizontalCollision) {
            break;
        }
    }

    // Callback only for closest (first) collision
    mOwner->OnCollision(responses);
}