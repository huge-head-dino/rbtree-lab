#include "rbtree.h"

#include <stdlib.h>

/* 1️⃣ 트리의 구조체를 생성하는 함수-- #️⃣test_init() */
rbtree *new_rbtree(void) {
  // 1) tree 구조체에 메모리를 동적할당한다.
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  // 2) nil 노드를 생성한다.
  // nil노드에도 메모리를 동적할당해준다. 아래 부분은 head 파일에 있는 node_t를 정의해놓은 것을 확인해보면 이해가 쉽다. 
  t-> nil = (node_t *)calloc(1, sizeof(node_t));
  // 트리 안에 있는 노드를 초기화 해주기 위해서 root에 nil을 넣는다.
  t-> root = t->nil;
  // nil 노드는 항상 검정색으로 설정해줘야한다. 
  // head파일에 가보면 typedef로 enum안에 레드랑 블랙을 color_t로 정의했고
  // typedef로 struct인 node_t 안에 color__t를 color로 정의했다. 따라서 t->nil->color로 받을 수 있게 된 것
  t-> nil ->color = RBTREE_BLACK;
  return t;
}





/* 2️⃣ RB tree 구조체가 heap에서 썼던 메모리를 반환하는 함수들-- #️⃣test_init() */

// 노드는 부모,왼쪽자식,오른쪽자식 3개의 정보를 가지고 있다.(주소가 아닌 color와 key제외)
// (확실x) 이건 node만 삭제하는 함수
void delete_node(rbtree *t, node_t *node)
{
  // 1) 노드의 왼쪽 자식이 nil이 아닐 경우 왼쪽 자식도 free
  if (node->left != t->nil)
    delete_node(t,node->left);
  // 2) 노드의 오른쪽 자식이 nil이 아닐 경우 오른쪽 자식도 free
  if (node->right != t->nil)
    delete_node(t,node->right);
  // 3) 현재 노드가 차지하고 있던 메모리를 반환하기
  free(node);
}

// (확실x) 이건 tree만 삭제하는 함수다. 삭제의 막타라고 보면 될듯
// node의 주소를 가리킬 때 t -> node 이렇게 가리키기 때문에 t를 먼저 삭제하면
// 가리킬 수 없게 된다. 따라서 안에 들어있는 node를 먼저 삭제하고 트리를 삭제한다. 
void delete_rbtree(rbtree *t) {
  //함수를 두개 써야함-- why? -- //
  //1) 노드하나를 만들어서 t의 root를 넣어준다.
  node_t * node = t->root;
  if (node != t->nil)
  //2) 그 노드가 nil이 아닐 경우 delete의 연장함수 실행
    delete_node(t, node);
  //3) 트리에 마지막으로 남아있는 nil node를 삭제
  // root는 메모리를 할당한 적이 없다. 그래서 nil만 삭제하는 것
  // 또한, new tree에서 맨처음에 node생성시 root에 nil 넣어줬기 때문에 결국엔 nil삭제가 root삭제
  free(t->nil);
  free(t);
}

/* 0️⃣ 회전과 색 변경을 담당하는 함수들 */
// [1]오른쪽으로 회전하는 함수//
void right_rotate(rbtree *t, node_t *node)
{
  // *️⃣ 사전준비 단계 //
  // 밑에 insert_fixup함수에서 선언했던 것처럼 부모와 조상을 선언해준다.
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  // insert_fixup과는 다른점이다. 오른쪽으로 회전하는 함수이기 때문에
  // node_right라는 노드를 만들어서 입력값으로 받은 node가 회전하는 쪽에 있는 자식을 담아둔다.
  node_t *node_right = node->right;

  // 0) 부모가 루트인 경우(좌측에 달린 자식이 회전하면): 현재 노드를 루트로 지정
  // 노드를 삭제했을 경우에만 쓰인다. => why? => 부모가 루트인 경우 삽입했을 때는 그냥 거기서 끝임.(쌍방울 형태)
  if (parent == t->root)
   t->root = node;
  else
  // 부모가 루트가 아니면
  { // 1-1) 입력값으로 받아온 노드의 조상을 부모로 교체
    
    if(grand_parent->left == parent) //조상의 왼쪽 자식이 부모였다면
      grand_parent->left = node;  // 조상의 왼쪽 자식은 이제 나
    else // 조상의 오른쪽 자식이 부모였다면
      grand_parent->right = node; // 조상의 오른쪽 자식은 이제 나
  }
  // *️⃣ 재연결 보수작업 //
  // 1. 조상의 자식은 이제 나야!
  // 2. 이제는 자식이 되어버린 (전)부모의 부모도 나지!
  // 3-1. 내 오른쪽 자식이었던 녀석들의 부모는 이제 내 (전)부모야.
  // 3-2. 내 (전)부모의 왼쪽 자식은 내 오른쪽 자식이었던 녀석들이야.(양방향 연결)
  node -> parent = grand_parent; // 1-2) 노드에 담긴 정보(누가 니 부모?)도 갱신(양방향 연결)
  parent->parent = node;         // 2-1) 이제 부모이던 놈의 부모는 입력받은 노드
  node-> right = parent;         // 2-2) 노드에 담긴 정보(누가 니 오른쪽 자식?)도 갱신(양방향 연결)
  node_right->parent = parent;   // 3-1) 
  parent->left = node_right;     // 3-2)
}

// [2]왼쪽으로 회전하는 함수//
void left_rotate(rbtree *t, node_t *node)
{
  // *️⃣ 사전준비 단계 //
  // 밑에 insert_fixup함수에서 선언했던 것처럼 부모와 조상을 선언해준다.
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  // 이번에는 왼쪽으로 회전하는 함수라서 입력받은 노드의 왼쪽에 달려있던 자식을 담아두는 그릇 형성
  node_t *node_left = node->left;

  // 아래의 과정은 right_rotate와 같다. (방향만 반대로)
  if (parent == t->root)
    t->root = node;
  else
  { // 1-1) 노드의 부모를 grand_parent로 변경
    if (grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }

  // *️⃣ 재연결 보수작업 //
  node->parent = grand_parent; // 1-2) 노드를 grand_parent의 자식으로 변경 (양방향 연결)
  parent->parent = node;       // 2-1) parent의 부모를 노드로 변경
  node->left = parent;         // 2-2) parent를 노드의 자식으로 변경 (양방향 연결)
  parent->right = node_left;   // 3-1) 노드의 자식의 부모를 parent로 변경
  node_left->parent = parent;  // 3-2) 노드의 자식을 부모의 자식으로 변경 (양방향 연결)
}

void exchange_color(node_t *x, node_t *y)
{
  // 대전제: x와 y의 색은 다르다. 그러니까 둘을 교환하지
  // 0) temp에다가 x의 색을 넣어놓고
  int temp = x->color;
  // 1) x의 색을 y의 색으로 바꾸고
  x->color = y->color;
  // 2) temp(처음 받아온 x의 색)가 BLACK이다라는 말이 참이면 y의 색을 검정으로 temp(처음 받아온 x의 색)가 BLACK이다라는 말이 거짓이면 y의 색을 빨강으로 바꾼다.
  // 즉, x를 y로 바꿨으니까 y에 원래 처음 받아온 x를 담겠다는 것이다.
  y->color = (temp == RBTREE_BLACK) ? RBTREE_BLACK: RBTREE_RED;

}


/* 3️⃣ 불균형을 복구하는 함수 --#️⃣test_insert_single() */

void rbtree_insert_fixup(rbtree *t, node_t *node)
{
  // *️⃣ 사전준비 단계 //
  //1) parent라는 노드를 하나 선언하고 인자로 받아온 노드의 부모로 정의한다.
  node_t *parent = node->parent;
  //2) 조상노드를 하나 선언하고 위에서 선언한 parent라는 노드의 부모로 정의한다.
  node_t *grand_parent = parent->parent;
  //3) 삼촌 노드를 하나 선언한다.
  node_t *uncle;
  //4) 입력값으로 받아온 노드가 왼쪽 자식인지 확인 (+숫자넣을거라 int형 변수로 받음)
  int iam_left = node == parent->left; 
  //5) 입력값으로 받아온 노드의 부모가 왼쪽 자식인지 확인
  int RU_left_parent;


  // *️⃣ 추가된 노드가 root노드인 경우: 
  // 삽입될 때 추가하는 노드의 색은 RED로 고정되어 있기 때문에 색을 검정으로 바꿔준다.
  // 규칙 #2 root노드의 색은 Black이어야 한다.
  if(node == t->root)
  {
    node->color = RBTREE_BLACK;
  // root노드 하나만 있는 상태에서 검정으로 바꾸면 밸런싱이 잡히는거니까 fixup 함수를 끝낸다.
    return;
  }

  // *️⃣ 부모가 BLACK인 경우: 그냥 그대로 두면 됨.(=밸런싱이 잡혀있으니 fixup함수를 끝내라)
  if (parent->color== RBTREE_BLACK)
    return;
  // 위 if문에 걸리지 않았다는 것은 곧, 부모가 빨간색이라는 것임.


  // 조상의 왼쪽 자식이 부모가 맞다 => 참
  // 조상의 왼쪽 자식이 부모가 아니다 => 거짓
  // 이 개념 그대로 가지고 삼항연산자 라인으로 내려감
  RU_left_parent = grand_parent -> left == parent;

  // 삼항연산자를 사용해서 uncle을 정의
  // 참이면 :의 좌측값을 uncle에 담기  or  거짓이면 :의 우측값을 uncle에 담기
  // 삼촌이 부모의 반대에 위치해야한다는 개념을 이해하면 쉬움
  uncle = (RU_left_parent) ? grand_parent->right : grand_parent->left;

  
  // *️⃣ [CASE 1]: 부모와 삼촌이 RED인 경우
  // 부모&삼촌과 할아버지의 색을 바꾼다.
  if (uncle->color == RBTREE_RED)
  {
    parent->color = RBTREE_BLACK;
    uncle->color = RBTREE_BLACK;
    grand_parent->color= RBTREE_RED;

    // 할아버지를 RED로 바꿨을 때, 할아버지의 조상이 RED가 되면 또 다시 불균형을 조정해줘야 하기 때문에 재귀로 돌린다.
    rbtree_insert_fixup(t, grand_parent);
    // 재귀로 돌렸으면 함수를 끝낸다.
    return;
  } 

  // @@@부모가 조상의 왼쪽일 때
  if (RU_left_parent) // RU_left_parent 변수가 참인 경우 즉, 부모가 조상의 왼쪽 노드인 경우
  {
    if (iam_left) // 삽입한 노드가 부모의 왼쪽 자식인 경우
    // [CASE 2]: 삼촌이 BLACK & 부모가 조상의 왼쪽 자식 & 입력 받은 노드가 부모의 왼쪽 자식
    // 유튜브 쉬운코드 영상 기준으로 펴진 case라고 볼 수 있음
    {
      //부모 기준으로 우측 회전
      right_rotate(t,parent);
      //회전 이후 부모와 자식 관계가 바뀌었다는 것을 고려
      //회전하고 색을 바꿔준다.
      exchange_color(parent, parent->right);
      //CASE 2 종료
      return;
    }

    // [CASE 3]: 삼촌이 BLACK & 부모가 left 자식 & 입력 받은 노드가 부모의 right자식인 경우
    left_rotate(t,node);
    right_rotate(t,node);
    exchange_color(node, node->right);
    return;
  }

  // @@@부모가 조상의 오른쪽일 때 (위에 if부모가 조상의 왼쪽일 때가 걸리지 않는다면 오른쪽이라는 뜻)
  if (iam_left) // 
  {
    //[CASE 3]: 삼촌이 BLACK & 부모가 right 자식 & 입력받은 노드가 부모의 left자식인 경우
    right_rotate(t, node);
    left_rotate(t, node);
    exchange_color(node, node->left);
    return;
  }

  //[CASE 2]: 삼촌이 BLACK & 부모가 right 자식 & 입력받은 노드가 부모의 right 자식인 경우
  left_rotate(t, parent);
  exchange_color(parent, parent->left);
  
}


/* 3️⃣ RB tree에 key값을 추가(=node를 추가)하고 불균형 복구를 요청하는 함수-- #️⃣test_insert_single() */

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // *️⃣새로운 노드를 만들어서 데이터를 넣고 트리에 집어넣을 준비를 한다.
  // 1) 빈 노드하나를 생성한다. 그리고 동적 메모리를 할당한다.
  node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
  // 2) 받아온 key값을 저장하고, 삽입하는 노드는 항상 빨강이므로 지정해준다.
  new_node-> key = key;
  new_node-> color = RBTREE_RED;
  // 3) 추가한 노드의 왼쪽과 오른쪽에 NIL을 붙여준다.
  new_node->left = new_node -> right = t->nil;

  // *️⃣새 노드를 삽입할 위치를 찾는 과정.
  // 0) current라는 탐색기를 만들어준다(퀵소트에서 ptr을 생각해라)
  // 탐색기는 root에서부터 값을 비교하면서 아래로 내려가야해서 처음에는 root를 넣어준다.
  node_t *current = t ->root;

  // 1) while문을 돌려서 새노드를 삽입할 위치를 찾아준다. 
  while(current != t->nil)
  {

  // 2) 큰 if문을 사용해서 탐색기를 대소관계를 이용하여 nil이 걸릴때까지 내려준다.
  // 3) 이후 탐색기에 nil이 걸리면 그 자리에 앞서 만든 new_node를 넣어준다.
    if(key < current-> key)
    {
      if (current->left == t->nil)
      {
        current-> left = new_node; // 탐색기에 nil이 걸리면 (=리프노드까지 오면) 그 자리에 앞서 새로 만든 new_node를 추가해준다.
        break;      
      }
      // 탐색기에 nil일 걸릴 때까지 left로 내려간다.
      current = current -> left; 

    }
    else
    {
      if (current->right == t->nil)
      {
      current-> right = new_node; // 탐색기에 nil이 걸리면 그 자리에 new_node를 추가해준다
      break;
      }
      current = current->right;
    }
  }
  
  // *️⃣ 노드를 양방향으로 연결시켜 주는 과정 + 노드가 없으면 루트노드를 넣어주는 과정
    // 1) 뉴 노드의 부모를 지정해준다.
    new_node->parent = current;
    
    // 2) root가 nil이면(현재 트리에 아무것도 없다면) 루트에 뉴노드를 넣어준다.
    // 첫번째 데이터가 들어올 때의 흐름: 트리에 들어오는 첫번째 데이터는 while을 거치지 않는다.
    // 따라서 첫 데이터가 들어왔을 때는 current에 root값이 들어있고, 그 root값은 nil이다.
    // 그 상황에서 2)를 실행하는 것. 
    if(current == t->nil)
      t ->root = new_node;

  // *️⃣불균형을 복구하는 함수를 실행한다.
    // 트리정보와 현재 새로 들어온 노드 값을 인자로 전달해준다.
    rbtree_insert_fixup(t, new_node);

  // insert 자체가 완료되었을 때는 new_node 값을 반환하는게 맞다.
  return new_node;
}

/* 4️⃣ key값에 해당하는 노드를 반환하는 함수 --#️⃣ find_single() */
node_t *rbtree_find(const rbtree *t, const key_t key) 
{
  // current라는 탐색기를 선언하고 root를 넣는다.
  node_t *current = t->root;

  // t가 nil에 걸릴 때까지 반복
  while (current != t->nil)
  {
    // 만약 key값이 current 탐색기에 걸린 노드의 값과 같다면 current를 반환
    if(key == current->key)
      return current;
    // 같지 않다면 타고타고 내려간다.
    else
      current = (key < current->key) ? current->left : current->right;
  }
  return NULL; //입력값으로 받은 key값을 가진 노드가 없을 때는 NULL을 반환

}

/* 5️⃣ key값에 해당하는 노드를 반환하는 함수 --#️⃣ minmax_suite */
node_t *rbtree_min(const rbtree *t) 
{
  // current 노드를 선언하고 루트를 집어넣는다. = current는 탐색기라고 생각
  node_t *current = t->root;
  // RBtree는 BST의 성질을 가지고 있기 때문에 최소값은 루트노드 기준으로 좌측을 타고 쭉 내려가면 된다.
  // nil노드가 나올 때까지 왼쪽으로 쭉 내려간다.
  while(current->left != t->nil)
    current = current->left;
  //current의 왼쪽 노드가 nil이라는 것은 현재 current 탐색기에 걸린 값이 최소값이라는 뜻이다.
  return current;
}
/* 6️⃣ key값에 해당하는 노드를 반환하는 함수 --#️⃣ minmax_suite */
node_t *rbtree_max(const rbtree *t) 
{
  // current 노드를 선언하고 루트를 집어넣는다. = current는 탐색기라고 생각
  node_t *current = t->root;
  // RBtree는 BST의 성질을 가지고 있기 때문에 최대값은 루트노드 기준으로 우측을 타고 쭉 내려가면 된다.
  // nil노드가 나올 때까지 오른쪽으로 쭉 내려간다.
  while(current->right != t->nil)
    current = current->right;
  //current의 오른쪽 노드가 nil이라는 것은 현재 current 탐색기에 걸린 값이 최대값이라는 뜻이다.
  return current;
}

/* 0️⃣ 키 값을 기준으로 다음 노드를 반환하는 함수 */
node_t *get_next_node(const rbtree *t, node_t *p)
{
  node_t * current = p-> right;
  if (current == t->nil) // 오른쪽 자식이 nil이면(=없으면)
  {
    current = p; // 다시 탐색기에 p를 담는다.
    while(1) //무한루프
    {
      if(current->parent->right == current) //current가 오른쪽 자식인 경우
        current = current->parent;          //cur 탐색기를 부모 노드로 이동하고
      else // current가 왼쪽 자식인 경우 
        return current->parent; // 현재 탐색기에 걸린 노드의 부모를 리턴한다
    }
  }
  while(current->left != t->nil) //왼쪽 자식이 있으면
    current = current->left;     //왼쪽 끝으로 이동
  return current;
}

// 9️⃣ 노드 삽입 후 불균형을 복구하는 함수//
// `parent`: extra_black이 부여된 노드의 부모
// `is_left`: extra_black이 부여된 노드가 왼쪽 자식인지 여부
void rbtree_erase_fixup(rbtree *t, node_t *parent, int is_left)
{
  // 삭제 후 대체한 노드가 RED (Red & Black): BLACK으로 변경
  node_t *extra_black = is_left ? parent->left : parent->right;
  if (extra_black->color == RBTREE_RED)
  {
    extra_black->color = RBTREE_BLACK;
    return;
  }

  node_t *sibling = is_left ? parent->right : parent->left;
  node_t *sibling_left = sibling->left;
  node_t *sibling_right = sibling->right;

  if (sibling->color == RBTREE_RED)
  { // [CASE D3] 형제가 RED
    if (is_left)
      left_rotate(t, sibling);
    else
      right_rotate(t, sibling);
    exchange_color(sibling, parent);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  node_t *near = is_left ? sibling_left : sibling_right;    // 형제의 자식 중 extra_black으로부터 가까운 노드
  node_t *distant = is_left ? sibling_right : sibling_left; // 형제의 자식 중 extra_black으로부터 먼 노드

  if (is_left && near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  { // [CASE D4] 형제가 BLACK, 형제의 가까운 자식이 RED, 형제의 더 먼 자식이 BLACK
    right_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  if (is_left && distant->color == RBTREE_RED)
  { // [CASE D5] 형제가 BLACK, 형제의 더 먼 자식이 RED
    left_rotate(t, sibling);
    exchange_color(sibling, parent);
    distant->color = RBTREE_BLACK;
    return;
  }

  if (near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
  { // [CASE D4] 형제가 BLACK, 형제의 가까운 자식이 RED, 형제의 더 먼 자식이 BLACK
    left_rotate(t, near);
    exchange_color(sibling, near);
    rbtree_erase_fixup(t, parent, is_left);
    return;
  }

  if (distant->color == RBTREE_RED)
  { // [CASE D5] 형제가 BLACK, 형제의 더 먼 자식이 RED
    right_rotate(t, sibling);
    exchange_color(sibling, parent);
    distant->color = RBTREE_BLACK;
    return;
  }

  // [CASE D2] 형제가 BLACK, 형제의 자식이 둘 다 BLACK
  sibling->color = RBTREE_RED;

  if (parent != t->root)
    rbtree_erase_fixup(t, parent->parent, parent->parent->left == parent);
}

/* 8️⃣ node를 삭제 */
int rbtree_erase(rbtree *t, node_t *delete) 
{
  node_t *remove; // 트리에서 삭제하는 노드
  node_t *remove_parent, *replace_node; // 삭제하는 노드의 부모노드와 대체노드도 선언
  int is_remove_black, is_remove_left;

  // 1) 삭제하는 노드인 delete를 삭제하고 그 자리를 대체할 successor인 replace노드를 찾는다.
  // 1-1) 삭제하는 노드인 delete 노드의 자식이 둘인 경우: delete의 키를 석세서의 키값으로 대체한다. 단, 이때 노드의 색은 delete의 색으로 유지해야한다.
  if(delete->left != t ->nil && delete->right != t->nil)
  {
    remove = get_next_node(t, delete); //석세서 노드 (기준:오른쪽 서브트리에서 가장 작은노드)
    replace_node = remove -> right; // 대체노드: 석세서 노드는 오른쪽 서브트리에서 가장 작은 노드 즉, 그보다 더 작은 왼쪽 자식은 있을 수가 없다. 자식이 있다면 그보다 큰 오른쪽 자식밖에 있을 수 없다.
    delete-> key = remove-> key; // delete의 키를 석세서 노드의 키값으로 대체한다. 색은 안건들면 그냥 유지된다.
  }
  else
  {// 1-2) delete 노드의 자식이 없거나 하나인 경우: delete 노드를 자식으로 대체, 노드의 색도 대체되는 노드의 색으로 변경
    remove = delete;
    // 대체할 노드: 자식이 있으면 자식노드로, 없으면 nil노드로 대체
    replace_node = (remove->right != t->nil) ? remove->right : remove->left;

  }
  remove_parent = remove->parent;


  // 2) remove 노드를 제거하기
  /* [CASE D1]: remove 노드가 루트인 경우 */
  if (remove == t->root)
  {
    t->root = replace_node;        // 대체할 노드를 트리의 루트로 지정
    t->root->color = RBTREE_BLACK; // 루트 노드는 항상 BLACK
    free(remove);
    return 0; // 불균형 복구 함수 호출 불필요 (제거 전 트리에 노드가 하나 혹은 두개이므로 불균형이 발생하지 않음)
  }

  // Step 2-1) 'remove의 부모'와 'remove의 자식' 이어주기
  is_remove_black = remove->color; // remove 노드 제거 전에 지워진 노드의 색 저장
  is_remove_left = remove_parent->left == remove;

  // Step 2-1-1) 자식 연결
  if (is_remove_left) // remove가 왼쪽 자식이었을 경우: remove 부모의 왼쪽에 이어주기
    remove_parent->left = replace_node;
  else // remove가 오른쪽 자식이었을 경우: remove 부모의 오른쪽에 이어주기
    remove_parent->right = replace_node;

  // Step 2-1-2) 부모도 연결 (양방향 연결)
  replace_node->parent = remove_parent;
  free(remove);

  /* [CASE D2~D6]: remove 노드가 검정 노드인 경우 */
  // Step 3) 불균형 복구 함수 호출
  if (is_remove_black)
    rbtree_erase_fixup(t, remove_parent, is_remove_left); 

  return 0;
}


/* 7️⃣ array로 변환 */
// 트리 t를 중위순회로 n번 순회한 결과를 arr에 담는다.
// 중위순회로 담아야하는 이유: BST의 트리의 특성을 살려서 root보다 작은걸 먼저 담아주고, root를 담고, root보다 큰걸 담아준다.

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  node_t * current = rbtree_min(t);
  arr[0] = current->key;
  for (int i=1; i<n; i++)
  {
    //1) root의 왼쪽을 다 보고
    if(current == t->nil) //current 탐색기가 nil에 걸리면 loop 탈출
      break;
    
    current = get_next_node(t,current); // 다음 노드로 이동
    if(current == t->nil)
      break;               //노드가 끝까지 탐색된 경우 loop 탈출
    arr[i] = current->key; //current 탐색기가 걸린 node의 key값을 arr에 담는다.
  }
  return 0;
}
