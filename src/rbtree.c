#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void)
{
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  // rbtree 데이터 타입의 사이즈 1개를 담을 수 있는 공간을 rbtree타입의 포인터 변수 t에 할당

  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));

  nilNode->color = RBTREE_BLACK;
  t->nil = nilNode;
  t->root = nilNode;

  return t;
}

void right_rotation(rbtree *t, node_t *y)
{
  // 1. x를 y의 왼쪽 자식으로 설정
  node_t *x = y->left;
  // 2. x의 right의 부모를 y로 연결
  y->left = x->right;
  if (x->right != t->nil)
    x->right->parent = y;
  // 3. y의 부모를 x의 부모와 연결(3가지 경우)
  x->parent = y->parent;
  if (y->parent == t->nil)
    t->root = x;
  else if (y == y->parent->left)
    y->parent->left = x;
  else
    y->parent->right = x;
  // 4. x와 y를 연결
  x->right = y;
  y->parent = x;
};

void left_rotation(rbtree *t, node_t *x)
{
  // 1. y를 x의 오른쪽 자식으로 설정
  node_t *y = x->right;
  // 2. y의 left의 부모를 x로 연결
  x->right = y->left;
  if (y->left != t->nil)
    y->left->parent = x;
  // 3. x의 부모를 y의 부모와 연결(3가지 경우)
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  // 4. x와 y를 연결
  y->left = x;
  x->parent = y;
}

void free_node(rbtree *t, node_t *x)
{
  // 후위 순회 방식으로 RB Tree 내의 노드 메모리 반환
  if (x->left != t->nil)
    free_node(t, x->left);
  if (x->right != t->nil)
    free_node(t, x->right);
  free(x);
}

void delete_rbtree(rbtree *t)
{
  if (t->root != t->nil)
    free_node(t, t->root);
  free(t->nil);
  free(t);
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  // uncle node - y
  node_t *y;
  // z의 부모가 red일 경우 문제 해결을 위해 반복
  while (z->parent->color == RBTREE_RED)
  {
    // ↙ z의 부모가 조부모의 왼쪽 자녀일 경우
    if (z->parent == z->parent->parent->left)
    {
      y = z->parent->parent->right;
      // 1️⃣ 삼촌이 red일 경우(부모도 red)
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        // 다음 반복 하게 되면 z->p->p에서 check
        z = z->parent->parent;
      }
      else
      {
        // 2️⃣ z가 부모의 오른쪽 자식, 삼촌이 black인 경우
        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotation(t, z);
        }
        // 3️⃣ z가 부모의 왼쪽 자식, 삼촌이 black인 경우
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotation(t, z->parent->parent);
      }
    }
    // ↘ z의 부모가 조부모의 오른쪽 자녀일 경우
    else
    {
      y = z->parent->parent->left;
      // 1️⃣ 삼촌이 red일 경우(부모도 red)
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        // 다음 반복 하게 되면 z->p->p에서 check
        z = z->parent->parent;
      }
      else
      {
        // 2️⃣ z가 부모의 왼쪽 자식, 삼촌이 black인 경우
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotation(t, z);
        }
        // 3️⃣ z가 부모의 오른쪽 자식, 삼촌이 black인 경우
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotation(t, z->parent->parent);
      }
    }
  }
  // root의 자녀들이 부모, 삼촌인 경우 root가 red가 될 수 있음!
  t->root->color = RBTREE_BLACK;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
// u의 부모와 u의 자녀를 연결해주는 함수,,
{
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x)
{
  node_t *w;
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    // 🅰 LEFT CASE
    if (x == x->parent->left)
    {
      w = x->parent->right;
      // 1️⃣ : x의 형제 w가 red인 경우
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotation(t, x->parent);
        w = x->parent->right;
      }

      // 2️⃣ : x의 형제 w는 black, w의 두 자식 모두 black
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // 3️⃣ : x의 형제 w는 black, w의 왼쪽 자녀 red, 오른쪽 자녀 black
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotation(t, w);
          w = w->parent;
        }
        // 4️⃣ : x의 형제 w는 black, w의 오른쪽 자녀 red
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotation(t, x->parent);
        x = t->root;
      }
    }
    // 🅱 RIGHT CASE
    else
    {
      w = x->parent->left;

      // 1️⃣ : x의 형제 w가 red인 경우
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotation(t, x->parent);
        w = x->parent->left;
      }

      // 2️⃣ : x의 형제 w는 black, w의 두 자식 모두 black
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // 3️⃣ : x의 형제 w는 black, w의 왼쪽 자녀 red, 오른쪽 자녀 black
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotation(t, w);
          w = x->parent->left;
        }
        // 4️⃣ : x의 형제 w는 black, w의 오른쪽 자녀 red
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotation(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

void subtree_to_array(const rbtree *t, node_t *curr, key_t *arr, size_t n, size_t *count)
{
  if (curr == t->nil)
    return;

  subtree_to_array(t, curr->left, arr, n, count);
  if (*count < n)
    arr[(*count)++] = curr->key;
  else
    return;
  subtree_to_array(t, curr->right, arr, n, count);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *y = t->nil;
  node_t *x = t->root;
  node_t *z = (node_t *)calloc(1, sizeof(node_t));

  z->key = key;

  // x가 값이 존재하는 한 값을 비교하며 삽입될 자리 찾기
  while (x != t->nil)
  {
    // y는 nil이 아닌 마지막 x
    y = x;
    if (z->key < x->key)
      x = x->left;
    else
      x = x->right;
  }

  // z와 부모의 관계 연결
  z->parent = y;
  if (y == t->nil)
    t->root = z;
  else if (z->key < y->key)
    y->left = z;
  else
    y->right = z;

  // z의 자녀와 색상 설정
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  // 삽입으로 인한 속성 위배 수정
  rbtree_insert_fixup(t, z);

  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *current = t->root;

  while (current != t->nil)
  {
    if (current->key == key)
      return current;

    if (current->key < key)
      current = current->right;
    else
      current = current->left;
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t)
// 직접 짜서 오류 있을 수..
{
  // x가 root 일 때부터 탐색 시작
  node_t *y;
  node_t *x = t->root;

  // root가 nil인 경우 NULL 반환
  if (x == t->nil)
    return NULL;

  // nil이 나올 때까지 왼쪽자녀 탐색, nil이 아닌 마지막 x값을 return값
  while (x != t->nil)
  {
    y = x;
    x = x->left;
  }

  return y;
}

node_t *rbtree_max(const rbtree *t)
// 직접 짜서 오류 있을 수..
{
  // x가 root일 때부터 탐색 시작
  node_t *y;
  node_t *x = t->root;

  // root가 nil인 경우 NULL반환
  if (x == t->nil)
    return NULL;

  // nil이 나올 때까지 오른쪽 자녀 탐색, nil이 아닌 마지막 x값을 return
  while (x != t->nil)
  {
    y = x;
    x = x->right;
  }
  return y;
}

int rbtree_erase(rbtree *t, node_t *z)
{
  node_t *y;
  node_t *x;
  color_t yOriginalColor;

  y = z;
  yOriginalColor = y->color;

  // z 보다 작은 노드 없을 경우 x(z->right)를 z->parent와 직접 연결
  if (z->left == t->nil)
  {
    x = z->right;
    rbtree_transplant(t, z, x);
  }
  // z 보다 큰 노드 없을 경우 x(z->left)를 z->parent와 직접 연결
  else if (z->right == t->nil)
  {
    x = z->left;
    rbtree_transplant(t, z, x);
  }
  // 그 외의 경우(z가 자녀 둘은 가진 경우)
  else
  {
    // successor y 찾기
    y = z->right;
    while (y->left != t->nil)
      y = y->left;

    // yOriginalColor 변수에 successor의 color 저장
    yOriginalColor = y->color;
    // x에 successor 오른쪽 자녀 저장
    x = y->right;

    // z와 successor가 원래 부모-자녀관계 였으면 successor의 right를 z에 연결
    if (y->parent == z)
      x->parent = y;
    // successor의 부모와 successor의 오른쪽 자녀 연결
    else
    {
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    // z의 부모와 successor 연결,
    // 원래 z에 연결되어 있던 왼쪽 서브트리를 y의 왼쪽 자녀로 연결
    // z->left 였던 y->left의 부모로 y연결
    // successor의 color에 기존 z의 color 연결
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  // 기존 successor의 색이 black이었으면 fixup!
  if (yOriginalColor == RBTREE_BLACK)
    rbtree_delete_fixup(t, x);

  // 삭제된 z 메모리 free
  free(z);

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  if (t->root == t->nil)
    return 0;

  size_t cnt = 0;
  subtree_to_array(t, t->root, arr, n, &cnt);

  return 0;
}
