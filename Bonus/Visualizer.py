#!/usr/bin/env python3
"""
visu-hex: A Pygame visualizer for 42 Lem-in (reads from stdin).

Usage:
    ./lem-in < ant_farm_map.txt | ./visu-hex

The script reads ALL input from stdin first, parses it, then opens a Pygame
window and visualizes the rooms, links and the ants moving step-by-step.

Input assumptions (adapted to your example):
- First non-empty line: number of ants (integer).
- Then lines describing rooms, possibly with markers:
    ##start
    <room_id> <x> <y>
    ...
    ##end
    <room_id> <x> <y>
- After rooms: link lines like "0-4" (room_id-room_id).
- Then move lines: each line contains space-separated moves like "L1-2 L2-3".
  (There may be blank lines between sections; move parsing stops at EOF.)
"""

import sys
import re
import pygame
import math
import time
from collections import defaultdict

# -------------------------
# Parsing utilities
# -------------------------

def parse_input(text):
    """
    Parse stdin text into:
    - num_ants: int
    - rooms: dict room_id -> (x, y)
    - start_id, end_id: optional (ints or None)
    - links: list of (id1, id2)
    - moves: list of steps, each step is list of (ant_id (int), room_id)
    """

    lines = [l.strip() for l in text.splitlines()]
    i = 0
    n = len(lines)

    # skip leading empties
    while i < n and lines[i] == "":
        i += 1
    if i >= n:
        raise ValueError("No input provided")

    # first line: number of ants
    try:
        num_ants = int(lines[i])
    except Exception as e:
        raise ValueError(f"Expected number of ants on first non-empty line, got: {lines[i]!r}") from e
    i += 1

    rooms = {}
    start_id = None
    end_id = None

    # parse rooms: lines that are either markers or "id x y" triple
    pending_marker = None  # "start" or "end" when we saw ##start/##end
    move_pattern = re.compile(r'^\s*(L\d+-\S+)')
    room_triple_pattern = re.compile(r'^\s*(\S+)\s+(-?\d+)\s+(-?\d+)\s*$')

    while i < n:
        line = lines[i]
        if line == "":
            i += 1
            continue
        # if line is a move, break (but don't check for digit-only links)
        if move_pattern.match(line):
            break
        if line == "##start":
            pending_marker = "start"
            i += 1
            continue
        if line == "##end":
            pending_marker = "end"
            i += 1
            continue

        m = room_triple_pattern.match(line)
        if m:
            room_id_raw, xs, ys = m.groups()
            room_id = str(room_id_raw)  # Force string
            x = int(xs)
            y = int(ys)
            rooms[room_id] = (x, y)
            if pending_marker == "start":
                start_id = room_id
            if pending_marker == "end":
                end_id = room_id
            pending_marker = None
            i += 1
            continue
        
        # If we reach here and the line contains a '-', it's likely a link
        if '-' in line and not move_pattern.match(line):
            break
        
        i += 1

    # parse links
    links = []
    while i < n:
        line = lines[i]
        if line.strip() == "":
            i += 1
            continue
        if move_pattern.match(line):
            break
        if '-' in line:
            parts = [p.strip() for p in line.strip().split('-')]
            if len(parts) == 2:
                a, b = parts
                # Convert to strings for consistency
                a = str(a)
                b = str(b)
                if a in rooms and b in rooms:   # only keep valid links
                    links.append((a, b))
                else:
                    print(f"Warning: link {a}-{b} references missing room")
        i += 1

    # parse moves
    moves = []
    move_token_re = re.compile(r'^L(\d+)-(\S+)$')
    while i < n:
        line = lines[i]
        i += 1
        if not line:
            continue
        tokens = line.split()
        step = []
        for t in tokens:
            mt = move_token_re.match(t)
            if mt:
                ant_id = int(mt.group(1))
                room_raw = mt.group(2)
                room_id = str(room_raw)  # Force string
                step.append((ant_id, room_id))
        if step:
            moves.append(step)

    return {
        "num_ants": num_ants,
        "rooms": rooms,
        "start_id": start_id,
        "end_id": end_id,
        "links": links,
        "moves": moves,
    }



# ------------------------- Visualization -------------------------#


# Some UI constants
WINDOW_W = 1000
WINDOW_H = 700
MARGIN = 60
ROOM_RADIUS = 16
ANT_RADIUS = 8
TOP_BAR_H = 48
FPS = 120
ANIM_DURATION = 0.45  # seconds for interpolation between steps
AUTO_DELAY = 0.8      # seconds pause between steps in autoplay

# Color helpers
def color_tuple(r, g, b):
    return (r, g, b)

BG_COLOR = color_tuple(240, 245, 250)  # soft background
ROOM_COLOR = color_tuple(230, 230, 235)
ROOM_BORDER = color_tuple(120, 125, 140)
TEXT_COLOR = color_tuple(30, 35, 40)
LINK_COLOR = color_tuple(170, 175, 185)
START_COLOR = color_tuple(140, 220, 140)  # green
END_COLOR = color_tuple(230, 140, 140)    # red

ANT_PALETTE = [
    (80, 120, 200),
    (200, 100, 140),
    (120, 200, 160),
    (220, 170, 80),
    (150, 120, 210),
    (200, 90, 70),
    (100, 200, 230),
    (170, 220, 120),
    (230, 130, 200),
    (120, 120, 120),
]

def get_ant_color(ant_id):
    return ANT_PALETTE[(ant_id - 1) % len(ANT_PALETTE)]

# Transform room coordinates to screen coordinates with scaling + margins
def compute_transform(rooms, w, h, margin=MARGIN, top_bar=TOP_BAR_H):
    xs = [p[0] for p in rooms.values()]
    ys = [p[1] for p in rooms.values()]
    if not xs or not ys:
        return lambda x, y: (margin, top_bar + margin)
    minx, maxx = min(xs), max(xs)
    miny, maxy = min(ys), max(ys)
    # avoid zero span
    spanx = max(1, maxx - minx)
    spany = max(1, maxy - miny)
    avail_w = w - 2 * margin
    avail_h = h - top_bar - 2 * margin
    sx = avail_w / spanx
    sy = avail_h / spany
    s = min(sx, sy)
    # center
    tx = margin - minx * s + (avail_w - spanx * s) / 2
    ty = top_bar + margin - miny * s + (avail_h - spany * s) / 2

    def transform(x, y):
        transformed = int(x * s + tx), int(y * s + ty)
        return transformed
    return transform


# Linear interpolation
def lerp(a, b, t):
    return a + (b - a) * t

# -------------------------
# Main visualization class
# -------------------------

class LemInVisualizer:
    def __init__(self, parsed):
        pygame.init()
        pygame.display.set_caption("Lem-in Visualizer")
        self.screen = pygame.display.set_mode((WINDOW_W, WINDOW_H))
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("Arial", 18)
        self.title_font = pygame.font.SysFont("Arial", 22, bold=True)

        self.num_ants = parsed["num_ants"]
        self.rooms = parsed["rooms"]  # id -> (x,y)
        self.start_id = parsed["start_id"]
        self.end_id = parsed["end_id"]
        self.links = parsed["links"]
        self.moves = parsed["moves"]

        # transform coordinates to screen
        self.transform = compute_transform(self.rooms, WINDOW_W, WINDOW_H)

        # positions: room_id -> screen coords
        self.room_pos = {rid: self.transform(x, y) for rid, (x, y) in self.rooms.items()}

        # adjacency: for drawing links (undirected)
        self.adj = defaultdict(list)
        for a, b in self.links:
            self.adj[a].append(b)
            self.adj[b].append(a)

        # ant positions: we track current room for each ant (room id or None)
        # initially all ants are assumed to be at start if start exists, else None
        self.ant_room = {}
        for aid in range(1, self.num_ants + 1):
            self.ant_room[aid] = self.start_id if self.start_id is not None else None

        # For animation: store previous positions and target positions (screen coords)
        self.ant_prev_pos = {aid: self._room_center(self.ant_room[aid]) for aid in self.ant_room}
        self.ant_target_pos = dict(self.ant_prev_pos)

        # step control
        self.current_step_index = 0  # steps consumed (0 means before any moves)
        self.total_steps = len(self.moves)
        self.anim_progress = 1.0  # 0.0..1.0
        self.is_animating = False
        self.last_anim_start = None

        # playback controls
        self.autoplay = False
        self.last_autoplay_time = 0

    def _room_center(self, room_id):
        if room_id is None:
            # arbitrary off-screen
            return (-100, -100)
        if room_id in self.room_pos:
            x, y = self.room_pos[room_id]
            # move ants slightly upward to avoid overlap with label
            return (x, y)
        else:
            return (-100, -100)

    def apply_step(self, step_idx):
        """
        Apply moves at step_idx (0-based) to set ant target rooms.
        This updates ant_prev_pos <- current positions and sets ant_target_pos
        """
        if step_idx < 0 or step_idx >= self.total_steps:
            return
        step_moves = self.moves[step_idx]
        # set prev pos to current target pos (where they are now)
        for aid in self.ant_room:
            self.ant_prev_pos[aid] = self.ant_target_pos[aid]

        # apply moves: each token L<ant>-<room>
        for ant_id, room_id in step_moves:
            # update logical position
            if ant_id in self.ant_room:
                self.ant_room[ant_id] = room_id
            else:
                # if ant id beyond declared number, add it
                self.ant_room[ant_id] = room_id

        # update target positions based on new ant rooms
        for aid in self.ant_room:
            self.ant_target_pos[aid] = self._room_center(self.ant_room[aid])

        # start animation
        self.is_animating = True
        self.anim_progress = 0.0
        self.last_anim_start = time.time()

    def step_forward(self):
        if self.current_step_index >= self.total_steps:
            return
        self.apply_step(self.current_step_index)
        self.current_step_index += 1

    def step_backward(self):
        # naive backward: reconstruct state from scratch up to step-1
        if self.current_step_index <= 0:
            return
        # rebuild ant positions from beginning
        for aid in range(1, self.num_ants + 1):
            self.ant_room[aid] = self.start_id if self.start_id is not None else None
            self.ant_target_pos[aid] = self._room_center(self.ant_room[aid])
            self.ant_prev_pos[aid] = self._room_center(self.ant_room[aid])
        # apply moves up to current_step_index-1
        for si in range(0, self.current_step_index - 1):
            for aid, rid in self.moves[si]:
                self.ant_room[aid] = rid
        # set current_step_index back by 1 and animate to that state (we'll set targets to current)
        self.current_step_index -= 1
        # set prev and target equal (no animation)
        for aid in self.ant_room:
            self.ant_prev_pos[aid] = self._room_center(self.ant_room[aid])
            self.ant_target_pos[aid] = self._room_center(self.ant_room[aid])
        self.is_animating = False
        self.anim_progress = 1.0

    def toggle_autoplay(self):
        self.autoplay = not self.autoplay
        self.last_autoplay_time = time.time()

    def handle_events(self):
        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                return False
            if ev.type == pygame.KEYDOWN:
                if ev.key == pygame.K_ESCAPE or ev.key == pygame.K_q:
                    return False
                if ev.key == pygame.K_SPACE:
                    # advance one step
                    self.step_forward()
                if ev.key == pygame.K_RIGHT:
                    self.step_forward()
                if ev.key == pygame.K_LEFT:
                    self.step_backward()
                if ev.key == pygame.K_a:
                    # toggle autoplay
                    self.toggle_autoplay()
                if ev.key == pygame.K_r:
                    # reset
                    self.reset()
        return True

    def reset(self):
        # reset to initial state
        for aid in range(1, self.num_ants + 1):
            self.ant_room[aid] = self.start_id if self.start_id is not None else None
            self.ant_prev_pos[aid] = self._room_center(self.ant_room[aid])
            self.ant_target_pos[aid] = self._room_center(self.ant_room[aid])
        self.current_step_index = 0
        self.is_animating = False
        self.anim_progress = 1.0
        self.autoplay = False

    def update_animation(self):
        if self.is_animating:
            now = time.time()
            t = (now - self.last_anim_start) / ANIM_DURATION
            if t >= 1.0:
                t = 1.0
                self.is_animating = False
                # finalize prev positions to target
                for aid in self.ant_prev_pos:
                    self.ant_prev_pos[aid] = self.ant_target_pos[aid]
            self.anim_progress = t

        # autoplay handling: if autoplay on and not animating and there are steps left, trigger next
        if self.autoplay and not self.is_animating:
            now = time.time()
            if now - self.last_autoplay_time >= AUTO_DELAY:
                if self.current_step_index < self.total_steps:
                    self.step_forward()
                self.last_autoplay_time = now

    def draw_links(self):
        for a, b in self.links:
            if a in self.room_pos and b in self.room_pos:
                pygame.draw.line(self.screen, LINK_COLOR, self.room_pos[a], self.room_pos[b], 2)
            else:
                print(f"Missing room for link: {a}-{b}")

    def draw_rooms(self):
        for rid, (x, y) in self.room_pos.items():
            color = ROOM_COLOR
            if self.start_id is not None and rid == self.start_id:
                color = START_COLOR
            elif self.end_id is not None and rid == self.end_id:
                color = END_COLOR
            pygame.draw.circle(self.screen, color, (x, y), ROOM_RADIUS)
            pygame.draw.circle(self.screen, ROOM_BORDER, (x, y), ROOM_RADIUS, 2)
            # draw label (id)
            label = self.font.render(str(rid), True, TEXT_COLOR)
            lw, lh = label.get_size()
            # put label to the right of the node
            self.screen.blit(label, (x + ROOM_RADIUS + 6, y - lh / 2))

    def draw_ants(self):
        # compute interpolated positions
        for aid in sorted(self.ant_room.keys()):
            prev = self.ant_prev_pos.get(aid, (-100, -100))
            targ = self.ant_target_pos.get(aid, (-100, -100))
            t = self.anim_progress
            ix = int(lerp(prev[0], targ[0], t))
            iy = int(lerp(prev[1], targ[1], t))
            color = get_ant_color(aid)
            pygame.draw.circle(self.screen, color, (ix, iy), ANT_RADIUS)
            # draw ant id small (optional)
            if ANT_RADIUS > 6:
                lbl = self.font.render(str(aid), True, (255,255,255))
                lw, lh = lbl.get_size()
                self.screen.blit(lbl, (ix - lw//2, iy - lh//2))

    def draw_top_bar(self):
        # background
        pygame.draw.rect(self.screen, BG_COLOR, (0, 0, WINDOW_W, TOP_BAR_H))
        # outline
        pygame.draw.line(self.screen, LINK_COLOR, (0, TOP_BAR_H-1), (WINDOW_W, TOP_BAR_H-1), 1)
        txt = f"Steps: {self.current_step_index} / {self.total_steps}"
        label = self.title_font.render(txt, True, TEXT_COLOR)
        self.screen.blit(label, (16, 12))

        controls = "Space/→: next  ←: back  A: autoplay  R: reset  Q/Esc: quit"
        c_label = self.font.render(controls, True, TEXT_COLOR)
        cw = c_label.get_width()
        self.screen.blit(c_label, (WINDOW_W - cw - 14, 15))

    def run(self):
        running = True
        while running:
            running = self.handle_events()
            self.update_animation()

            # paint background
            self.screen.fill(BG_COLOR)

            # draw links, rooms, ants, top bar
            self.draw_links()
            self.draw_rooms()
            self.draw_ants()
            self.draw_top_bar()

            pygame.display.flip()
            self.clock.tick(FPS)

        pygame.quit()

# -------------------------
# Entry point
# -------------------------

def main():
    raw = sys.stdin.read()
    if not raw:
        print("Error: expected input from stdin. Usage: ./lem-in < path_to_map.txt | python3 Bonus/Visualizer", file=sys.stderr)
        sys.exit(1)
    try:
        parsed = parse_input(raw)
    except Exception as e:
        print("Failed to parse input:", e, file=sys.stderr)
        sys.exit(2)

    vis = LemInVisualizer(parsed)
    vis.run()

if __name__ == "__main__":
    main()