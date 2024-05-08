import pygame
import random

# based on: https://github.com/SinisterSup/PongGame-Simulator
pygame.init()



WIDTH, HEIGHT = 800, 600
WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("My Pong Game")

FPS = 60

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GREY = (143,188,143)

PADDLE_HEIGHT, PADDLE_WIDTH = 100,42 
BALL_RADIUS = 7

WINNING_SCORE = 10
SCORE_FONT = pygame.font.SysFont("comicsans", 45)

#Object Paddle
class Paddle:
    COLOR = WHITE
    VEL = 8

    def __init__(self, x, y, width, height, right):
        self.x = self.original_x = x
        self.y = self.original_y = y
        self.width = width
        self.height = height
        self.right = right

    def draw(self, win):
        # pygame.draw.rect(win, self.COLOR, (self.x, self.y, self.width, self.height))
        if not self.right:
            pygame.draw.polygon(win, self.COLOR, ((self.x,self.y),(self.x,self.y+self.height),(self.x+self.width,self.y+(self.height/2))))
        else:
            pygame.draw.polygon(win, self.COLOR, ((self.x,self.y),(self.x,self.y+self.height),(self.x-self.width,self.y+(self.height/2))))

    def move(self, up = True, scale = 1):
        if up:
            self.y -= scale * self.VEL
        else:
            self.y += scale* self.VEL

    def reset_paddle(self):
        self.x = self.original_x
        self.y = self.original_y

# Object BALL
class Ball:
    MAX_VEL = 8
    COLOR = WHITE

    def __init__(self, x, y, radius):
        self.x = self.original_x = x
        self.y = self.original_y = y
        self.radius = radius
        self.x_vel = self.MAX_VEL
        self.y_vel = 0

    def draw(self, win):
        pygame.draw.circle(win, self.COLOR, (self.x, self.y), self.radius)

    def move(self):
        self.x += self.x_vel
        self.y += self.y_vel

    def reset(self):
        self.x = self.original_x
        self.y = self.original_y
        self.y_vel = 0
        self.x_vel *= -1

# Draw on a Window Using PyGame
def draw(wind, paddles, ball, left_score = 0, right_score = 0):
    wind.fill(BLACK)

    left_score_text = SCORE_FONT.render(f'{left_score}', 1, WHITE)
    right_score_text = SCORE_FONT.render(f'{right_score}', 1, WHITE)
    wind.blit(left_score_text, (WIDTH//4 - left_score_text.get_width()//2, 20))
    wind.blit(right_score_text, (WIDTH*3//4 - right_score_text.get_width()//2, 20))

    for paddle in paddles:
        paddle.draw(wind)

    for i in range(10, HEIGHT, HEIGHT//20):
        if i%2 == 1:
            continue
        pygame.draw.rect(wind, GREY, (WIDTH//2 - 5, i, 5, HEIGHT//20))

    ball.draw(wind)

    pygame.display.update()

def handle_paddle_movement(keys, left_paddle, right_paddle, ball_x, ball_y, ball_yvel, ball_xvel):
    scale = 0.25
    if ball_x >= 75:
        scale = 2

    ball_x += random.randint(-15,15)
    ball_y += random.randint(-15,15)

    val = left_paddle.height/2
    if ball_xvel < 0 and left_paddle.y - left_paddle.VEL + val >= ball_y - ball_yvel and left_paddle.y - left_paddle.VEL - 4 >= 0:
        left_paddle.move(up = True, scale = scale)
        
    if ball_xvel < 0 and left_paddle.y + left_paddle.VEL + val<= ball_y - ball_yvel and left_paddle.y + left_paddle.VEL + left_paddle.height + 4 <= HEIGHT:
        left_paddle.move(up = False, scale = scale)

    if keys[pygame.K_UP] and right_paddle.y - right_paddle.VEL - 4 >= 0:
        right_paddle.move(up = True)
    if keys[pygame.K_DOWN] and right_paddle.y + right_paddle.VEL + right_paddle.height + 4 <= HEIGHT:
        right_paddle.move(up = False)

def handle_collision(ball, left_paddle, right_paddle):
    # Collision with Ceiling :-
    ceil = random.random()
    if ball.y + ball.radius >=  HEIGHT:
        ball.y_vel -= ceil
        ball.y_vel *= -1
    elif ball.y - ball.radius <= 0:
        ball.y_vel += ceil
        ball.y_vel *= -1
    

    # left_paddle Collision
    if ball.x_vel < 0:
        if ball.y >= left_paddle.y and ball.y <= left_paddle.y + left_paddle.height:
            if ball.x - ball.radius <= left_paddle.x + left_paddle.width:
                ball.x_vel *= -1

                middle_y = left_paddle.y + left_paddle.height / 2
                difference_in_y = middle_y - ball.y
                reduction_factor = (left_paddle.height / 2) / ball.MAX_VEL
                y_vel = difference_in_y / reduction_factor
                ball.y_vel = -1 * y_vel

    # right_paddle Collision
    else:
        if ball.y >= right_paddle.y and ball.y <= right_paddle.y + right_paddle.height:
            if ball.x + ball.radius >= right_paddle.x-right_paddle.height:
                ball.x_vel *= -1

                middle_y = right_paddle.y + right_paddle.height / 2
                difference_in_y = middle_y - ball.y
                reduction_factor = (right_paddle.height / 2) / ball.MAX_VEL
                y_vel = difference_in_y / reduction_factor
                ball.y_vel = -1 * y_vel

def main():
    run = True
    clock = pygame.time.Clock()

    left_paddle = Paddle(10, HEIGHT//2 - PADDLE_HEIGHT//2, PADDLE_WIDTH, PADDLE_HEIGHT, False)
    right_paddle = Paddle(WIDTH - 10, HEIGHT//2 - PADDLE_HEIGHT//2, PADDLE_WIDTH, PADDLE_HEIGHT, True)
    ball = Ball(WIDTH//2, HEIGHT//2, BALL_RADIUS)

    left_score, right_score = 0, 0

    while run:
        clock.tick(FPS)
        draw(WIN, [left_paddle, right_paddle], ball, left_score, right_score)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
                break

        keys = pygame.key.get_pressed()
        handle_paddle_movement(keys, left_paddle, right_paddle, ball.x, ball.y, ball.y_vel, ball.x_vel)

        ball.move()
        handle_collision(ball, left_paddle, right_paddle)

        if ball.x < 0:
            right_score += 1
            ball.reset()
        elif ball.x > WIDTH:
            left_score += 1
            ball.reset()

        won = False
        if left_score >= WINNING_SCORE:
            won = True
            win_text = "Left Player Won!..YAY!"
        elif right_score >= WINNING_SCORE:
            won = True
            win_text = "Right Player Won!..YAY!"

        if won:
            text = SCORE_FONT.render(win_text, 1, WHITE)
            WIN.blit(text, (WIDTH//2 - text.get_width()//2, HEIGHT//2 - text.get_height()//2))
            pygame.display.update()
            pygame.time.delay(4000)
            ball.reset()
            left_paddle.reset_paddle()
            right_paddle.reset_paddle()
            left_score, right_score = 0, 0


    pygame.quit()

if __name__ == '__main__':
    main()
