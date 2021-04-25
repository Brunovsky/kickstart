#ifndef FRAC_QUICKHULL3D_HPP
#define FRAC_QUICKHULL3D_HPP

#include "../formatting.hpp"
#include "point3d.hpp" // Point3d, Plane

// *****

/**
 * 3D Quickhull for rational points.
 * All degenerate cases are handled:
 *   - 2+ points incident ==> all but one will be completely ignored
 *   - 3+ points collinear ==> handled properly, face edges may be collinear
 *   - 4+ points coplanar ==> handled properly, faces can have >3 points
 *
 * The convex hull generated
 *   - is not simple nor complete: faces whose sides have multiple collinear points
 *       might include none, any or all of those "interior" points.
 *   - is not canonical: the output hull's faces are in arbitrary order, and each face
 *       starts at an arbitrary point.
 * To address these issues see hull3d_utils.hpp
 *
 * Complexity: O(N log N) expected, O(N^2) worst-case
 * Reference: https://github.com/mauriciopoppe/quickhull3d
 *
 * Note: I do not intend for this to be used directly. It is a proof of concept
 * implementation with perfect precision that can be a reliable hull algorithm for
 * floating point implementations. It has considerable overflow issues with frac even
 * for small fractions (e.g. numerator>=50, denominator>=4 already causes overflows).
 *
 * Usage:
 *     vector<P> points = {P(1,2,3), ...};
 *     frac_quickhull3d qh(points);
 *     bool ok = qh.compute();              // returns false if all points are coplanar
 *     auto hull = qh.extract_hull();       // points 0-indexed in the faces
 *     for (auto face : hull) {
 *         for (int v : face) {
 *             ... // points[v]
 *         }
 *     }
 */
template <typename F>
struct frac_quickhull3d {
    using P = Point3d<F>;
    using Hull = vector<vector<int>>;
    struct Face;

    static inline constexpr int VISIBLE = 0, DELETED = 1;

    struct Edge {
        Face* face = nullptr;
        Edge *next, *prev, *opposite = nullptr;
        const int vertex; // [vertex]---edge-->[next->vertex]
        explicit Edge(int v, Face* face) : face(face), vertex(v) {}
        static void marry(Edge* a, Edge* b) { a->opposite = b, b->opposite = a; }
        static void link(Edge* a, Edge* b) { a->next = b, b->prev = a; }
    };

    struct Face {
        Plane<F> plane; // not normalized
        Edge* edge;     // an arbitrary edge in the face; a particular one initially
        int mark = VISIBLE, outside = 0, id;
        explicit Face(int id) : id(id) {}
    };

    int N;                          // number of points
    vector<P> points;               // points[1..N] from input
    vector<unique_ptr<Face>> faces; // active faces list
    vector<int> eye_prev, eye_next, open;
    vector<Face*> eye_face, new_faces, old_faces;
    vector<Edge*> horizon;

    frac_quickhull3d(const vector<P>& input)
        : N(input.size()), points(N + 1), eye_prev(N + 1, 0), eye_next(N + 1, 0),
          open(N + 1, 0), eye_face(N + 1) {
        copy(begin(input), end(input), begin(points) + 1);
    }

    // Eye tables subroutines

    void link_open(int v) { open[v] = open[0], open[0] = v; }
    void link_eye(int u, int v) { eye_next[u] = v, eye_prev[v] = u; }
    void clear_open() { open[0] = 0; }
    void clear_eye(int v) { eye_face[v] = nullptr, eye_prev[v] = eye_next[v] = 0; }

    void add_eye(int v, Face* face) {
        assert(v && !eye_face[v]);
        eye_face[v] = face;
        link_eye(eye_prev[face->outside], v);
        link_eye(v, face->outside);
        face->outside = v;
        return;
        if (face->outside != 0) {
            auto vdist = face->plane.signed_planedist2(points[v]);
            auto odist = face->plane.signed_planedist2(points[face->outside]);
            if (vdist < odist) {
                link_eye(v, eye_next[face->outside]);
                link_eye(face->outside, v);
                return;
            }
        }
        link_eye(eye_prev[face->outside], v);
        link_eye(v, face->outside);
        face->outside = v;
    }

    int find_next_eye(Face* face) {
        assert(face->mark == VISIBLE && face->outside != 0);
        auto maxdist = face->plane.planedist2(points[face->outside]);
        int best = face->outside, v = eye_next[face->outside];
        while (v && eye_face[v] == face) {
            auto dist = face->plane.planedist2(points[v]);
            if (maxdist < dist) {
                maxdist = dist, best = v;
            }
            v = eye_next[v];
        }
        return best;
    }

    void remove_eye(int v) {
        assert(v && eye_face[v]);
        Face* face = eye_face[v];
        if (face->outside == v)
            face->outside = eye_next[v];

        link_eye(eye_prev[v], eye_next[v]);
        clear_eye(v);
    }

    void remove_all_eyes(Face* face) {
        int v = face->outside, head = eye_prev[face->outside];
        while (v && eye_face[v] == face) {
            int next = eye_next[v];
            clear_eye(v);
            link_open(v);
            v = next;
        }
        link_eye(head, v);
    }

    // Face subroutines

    /**
     * Create a new triangular face for vertices v0, v1, v2.
     * edges: 0={v0 to v1}                v2
     *        1={v1 to v2}              2    1
     *        2={v2 to v0}.          v0 -> 0 -> v1
     */
    auto add_face(int v0, int v1, int v2) {
        Face* face = faces.emplace_back(make_unique<Face>(faces.size())).get();

        auto e0 = new Edge(v0, face);
        auto e1 = new Edge(v1, face);
        auto e2 = new Edge(v2, face);
        Edge::link(e0, e1), Edge::link(e1, e2), Edge::link(e2, e0);
        face->edge = e0;

        face->plane = Plane<F>(points[v0], points[v1], points[v2]);
        assert(!face->plane.is_degenerate());
        return face;
    }

    /**
     * Create a new triangular face for vertices eye, edge->vertex and edge->next->vertex.
     * edges: 0={edge->vertex to next->vertex}           eye
     *        1={next->vertex to eye}                  2     1
     *        2={eye to edge->vertex}             edge -> 0 -> next
     * This invalidates the given horizon edge
     */
    auto add_adjoining_face(int eye, Edge* edge) {
        auto face = add_face(edge->vertex, edge->next->vertex, eye);
        Edge::marry(face->edge, edge->opposite);
        return face;
    }

    void make_simplex_faces(int v0, int v1, int v2, int v3) {
        // v3 is above plane(v0,v1,v2)
        auto face0 = add_face(v0, v2, v1);
        auto face1 = add_face(v3, v0, v1);
        auto face2 = add_face(v3, v1, v2);
        auto face3 = add_face(v3, v2, v0);
        auto e00 = face0->edge, e01 = e00->next, e02 = e00->prev;
        auto e10 = face1->edge, e11 = e10->next, e12 = e10->prev;
        auto e20 = face2->edge, e21 = e20->next, e22 = e20->prev;
        auto e30 = face3->edge, e31 = e30->next, e32 = e30->prev;
        Edge::marry(e00, e31), Edge::marry(e01, e21), Edge::marry(e02, e11);
        Edge::marry(e10, e32), Edge::marry(e20, e12), Edge::marry(e30, e22);
    }

    void add_horizon_faces(int eye) {
        int H = horizon.size();
        for (Edge* edge : horizon) {
            new_faces.emplace_back(add_adjoining_face(eye, edge));
        }
        horizon.clear();
        for (int i = 0; i + 1 < H; i++) {
            Edge::marry(new_faces[i]->edge->next, new_faces[i + 1]->edge->prev);
        }
        Edge::marry(new_faces[H - 1]->edge->next, new_faces[0]->edge->prev);
    }

    void mark_face_for_deletion(Face* face) {
        assert(face && face->mark == VISIBLE);
        face->mark = DELETED, old_faces.push_back(face);
    }

    void delete_face(Face* face) {
        assert(face->mark == DELETED && face->edge != nullptr);
        int id = face->id;
        Edge* u = face->edge;
        do {
            Edge* v = u->next;
            delete u;
            u = v;
        } while (u && u != face->edge);
        swap(faces[id], faces.back());
        faces[id]->id = id;
        faces.pop_back();
    }

    void delete_old_faces() {
        for (Face* face : old_faces) {
            delete_face(face);
        }
        old_faces.clear();
    }

    // Edge subroutines (merging)

    bool should_merge(Edge* edge) const {
        return edge->face->plane == edge->opposite->face->plane;
    }

    void merge_faces(Edge* edge, bool recurse = true) {
        assert(should_merge(edge));

        Face *face = edge->face, *oface = edge->opposite->face;
        Edge *a = edge->prev, *c = edge->opposite->prev;
        Edge *b = edge->next, *d = edge->opposite->next;

        // merge edge->opposite->face (with no eyes) into edge->face
        assert(oface->outside == 0);
        assert(face->mark == VISIBLE && oface->mark == VISIBLE);
        //        a \         face -->     / b
        //        ^  * ---- *[----]* ---- *  v
        //        d /     <-- oface        \ c

        // Find a, b, c, d from the illustration
        while (a->opposite->face == oface)
            a = a->prev, d = d->next;
        while (b->opposite->face == oface)
            b = b->next, c = c->prev;

        face->edge = b;

        // Transfer edges from oface into face
        for (Edge* other = d; other != c->next; other = other->next)
            other->face = face;

        mark_face_for_deletion(oface);
        oface->edge = c->next;
        Edge::link(d->prev, oface->edge);
        Edge::link(a, d), Edge::link(c, b);

        if (recurse && should_merge(a))
            merge_faces(a, true);
        if (recurse && should_merge(b))
            merge_faces(b, true);
    }

    void merge_new_faces() {
        for (Face* face : new_faces) {
            if (face->mark == VISIBLE) {
                if (should_merge(face->edge)) {
                    merge_faces(face->edge->opposite);
                } else if (should_merge(face->edge->next)) {
                    merge_faces(face->edge->next->opposite);
                }
            }
        }
        new_faces.erase(remove_if(begin(new_faces), end(new_faces),
                                  [](Face* face) { return face->mark == DELETED; }),
                        end(new_faces));
    }

    // Main routines

    /**
     * Create initial (maximal tetrahedral) simplex with four non-coplanar points and
     * create the four faces supporting it. Then set eye_face[v] for each v as the
     * hull face seen from v that is furthest from it (or leave null if none exists,
     * meaning v is inside the hull already).
     * Fails and returns false if all points are coplanar. Otherwise returns true
     * with 4 faces forming a tetrahedron.
     */
    bool initialize_simplex() {
        // compute extreme values along each axis
        array<int, 3> minvert = {1, 1, 1}, maxvert = {1, 1, 1};

        for (int v = 2; v <= N; v++) {
            for (int d = 0; d < 3; d++) {
                if (points[minvert[d]][d] > points[v][d]) {
                    minvert[d] = v;
                }
                if (points[maxvert[d]][d] < points[v][d]) {
                    maxvert[d] = v;
                }
            }
        }

        int v0 = 0, v1 = 0, v2 = 0, v3 = 0;
        F maxdist = 0;

        // select v0, v1 such that dist2(v0, v1) is maximal (furthest pair along an axis)
        for (int d = 0; d < 3; d++) {
            auto dist = points[maxvert[d]][d] - points[minvert[d]][d];
            if (maxdist < dist) {
                maxdist = dist, v0 = minvert[d], v1 = maxvert[d];
            }
        }
        if (!v0 || !v1) {
            return false;
        }

        // select v2 such that linedist2(v2, v0, v1) is maximum (furthest from line)
        maxdist = 0;
        for (int v = 1; v <= N; v++) {
            if (v != v0 && v != v1) {
                auto dist = linedist2(points[v], points[v0], points[v1]);
                if (maxdist < dist) {
                    maxdist = dist, v2 = v;
                }
            }
        }
        if (!v0 || !v1 || !v2) {
            return false;
        }

        // select v3 such that base.planedist2(v3) is maximum (furthest from plane)
        Plane<F> base(points[v0], points[v1], points[v2]);
        maxdist = 0;
        for (int v = 1; v <= N; v++) {
            if (v != v0 && v != v1 && v != v2) {
                auto dist = base.planedist2(points[v]);
                if (maxdist < dist) {
                    maxdist = dist, v3 = v;
                }
            }
        }
        if (!v0 || !v1 || !v2 || !v3) {
            return false;
        }

        if (base.planeside(points[v3]) == -1) {
            make_simplex_faces(v0, v2, v1, v3);
        } else {
            make_simplex_faces(v0, v1, v2, v3);
        }

        for (int v = 1; v <= N; v++) {
            if (v != v0 && v != v1 && v != v2 && v != v3) {
                maxdist = 0;
                Face* maxface = nullptr;
                for (auto&& face : faces) {
                    auto dist = face->plane.signed_planedist2(points[v]);
                    if (maxdist < dist) {
                        maxdist = dist, maxface = face.get();
                    }
                }
                if (maxface) {
                    add_eye(v, maxface);
                }
            }
        }

        return true;
    }

    void extend_simplex() {
        while (eye_next[0]) {
            int eye = find_next_eye(eye_face[eye_next[0]]);
            add_vertex_to_hull(eye);
        }
    }

    void compute_horizon(int eye, Edge* cross, Face* face) {
        assert(eye && face && face->mark == VISIBLE && (!cross || cross->face == face));
        remove_all_eyes(face);
        mark_face_for_deletion(face);

        Edge* edge = cross ? cross->next : face->edge;
        cross = cross ? cross : face->edge;
        do {
            Edge* opposite = edge->opposite;
            if (opposite->face->mark == VISIBLE) {
                if (opposite->face->plane.planeside(points[eye]) == 1) {
                    compute_horizon(eye, opposite, opposite->face);
                } else {
                    horizon.push_back(edge);
                }
            }
            edge = edge->next;
        } while (edge != cross);
    }

    void resolve_unassigned_points() {
        for (int v = open[0]; v; v = open[v]) {
            F maxdist = 0;
            Face* maxface = nullptr;
            for (Face* face : new_faces) {
                F dist = face->plane.signed_planedist2(points[v]);
                if (maxdist < dist) {
                    maxdist = dist, maxface = face;
                }
            }
            if (maxface) {
                add_eye(v, maxface);
            }
        }
        for (int v = open[0], next; v != 0; v = next) {
            next = open[v], open[v] = 0;
        }
        clear_open();
        new_faces.clear();
    }

    void add_vertex_to_hull(int eye) {
        Face* face = eye_face[eye];
        remove_eye(eye);
        compute_horizon(eye, nullptr, face);
        add_horizon_faces(eye);
        merge_new_faces();
        resolve_unassigned_points();
        delete_old_faces();
        verify_invariants();
    }

    // Interface

    bool compute() {
        bool ok = initialize_simplex();
        if (ok) {
            extend_simplex();
        }
        return ok;
    }

    auto extract_hull(int s = 0) const {
        vector<vector<int>> hull(faces.size());
        for (auto&& face : faces) {
            Edge* edge = face->edge;
            do {
                int v = edge->vertex - 1 + s;
                hull[face->id].push_back(v);
                edge = edge->next;
            } while (edge != face->edge);
        }
        return hull;
    }

#ifdef DEBUG_HULL3D
#define my_ass(test, msg) assert(test&& bool(msg))

    void debug_iteration(int iteration) {
        print("===== iteration {}, nfaces={}\n", iteration, faces.size());
        auto ordered_faces = extract_hull(0);

        if (eye_next[0]) {
            int eye = eye_next[0], id = eye_face[eye]->id;
            print("eye={} -> {}    face={} -> [{}]\n", eye - 1, points[eye], id,
                  ordered_faces[id]);
        } else {
            print("DONE\n");
        }
        debug();
    }

    void debug_eyes() {
        print("      nodes: NIL");
        for (int v = 1; v <= N; v++) {
            print(" {:>2}", v - 1);
        }
        cout << endl;

        print("   eye_face: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", eye_face[v] ? to_string(eye_face[v]->id) : "");
        }
        cout << endl;

        print("   eye_next: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", eye_next[v] ? to_string(eye_next[v] - 1) : "");
        }
        cout << endl;

        print("   eye_prev: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", eye_prev[v] ? to_string(eye_prev[v] - 1) : "");
        }
        cout << endl;

        print("outsides:");
        for (int f = 0; f < int(faces.size()); f++) {
            print("  {}->{}", f, faces[f]->outside);
        }
        cout << endl;
    }

    void debug_faces() {
        auto ordered_faces = extract_hull(0);

        print("faces:");
        for (int i = 0, f = ordered_faces.size(); i < f; i++) {
            print(" {}=[{}]", i, ordered_faces[i]);
        }
        cout << endl;
    }

    void debug_unassigned() {
        print(" open: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", open[v] ? to_string(open[v] - 1) : "");
        }
        cout << endl;
    }

    void debug_horizon() {
        print("horizon:\n");
        for (Edge* edge : horizon) {
            assert(edge && edge->face->mark == DELETED);
            print("   {} (face={})\n", edge->vertex - 1, edge->face->id);
        }
        cout << flush;
    }

    void debug() {
        debug_faces();
        debug_eyes();
        debug_unassigned();
        debug_horizon();
    }

    void verify_horizon() {
        unordered_set<Edge*> horizonset(begin(horizon), end(horizon));
        unordered_set<Face*> oldset(begin(old_faces), end(old_faces));

        for (Edge* edge : horizon) {
            Face *face = edge->face, *oface = edge->opposite->face;
            my_ass(face->mark == DELETED && oldset.count(face),
                   "horizon edge is apparently visible");
            my_ass(oface->mark == VISIBLE && !oldset.count(oface),
                   "horizon opposite edge is apparently not visible");
        }
    }

    void verify_active_faces() {
        unordered_set<Face*> oldset(begin(old_faces), end(old_faces));
        for (int f = 0, Q = faces.size(); f < Q; f++) {
            Face* face = faces[f].get();
            my_ass(face, "faces[] contains null list");
            my_ass(face->edge, "face has null head edge");
            my_ass(face->edge->face == face, "face head edge assigned to wrong face");
            my_ass(faces[face->id].get() == face, "face in wrong position in faces[]");
            my_ass((face->mark == VISIBLE || oldset.count(face)), "not in old_faces");
            my_ass(face->outside >= 0 && face->outside <= N, "bad face outside eye");
        }
    }

    void verify_old_faces() {
        unordered_set<Face*> oldset(begin(old_faces), end(old_faces));

        for (Face* face : old_faces) {
            my_ass(face->mark == DELETED, "old face is not deleted");
        }
    }

    void verify_eyes() {
        vector<unordered_set<int>> eyes(faces.size());

        for (int f = 0, Q = faces.size(); f < Q; f++) {
            Face* face = faces[f].get();
            int eye = face->outside;
            while (eye && eye_face[eye] == face) {
                eyes[face->id].insert(eye);
                eye = eye_next[eye];
            }
        }

        for (int v = 1; v <= N; v++) {
            if (eye_face[v] == nullptr) {
                my_ass(eye_prev[v] == 0 && eye_next[v] == 0, "eye was not cleared");
                continue;
            }
            Face* face = eye_face[v];
            my_ass(face && faces[face->id].get() == face, "bad eye face");
            my_ass(face && face->mark == VISIBLE, "bad eye face");

            my_ass(eyes[face->id].count(v), "eye not reachable from face");
            my_ass(eye_prev[eye_next[v]] == v, "fucked up eye list next");
            my_ass(eye_next[eye_prev[v]] == v, "fucked up eye list prev");
        }
    }

    void verify_edges(bool nocoplanar = false) {
        for (int f = 0, Q = faces.size(); f < Q; f++) {
            Face* face = faces[f].get();

            vector<Edge*> edges;
            Edge* edge = face->edge;
            do {
                my_ass(edge->face == face, "edge assigned to wrong face");
                my_ass(edge->prev->next == edge, "edge incorrect prev->next");
                my_ass(edge->next->prev == edge, "edge incorrect next->prev");
                my_ass(edge->opposite->opposite == edge, "edge incorrect opposite");
                my_ass(edge->opposite->face != face, "edge opposite shares face");
                my_ass(edge->next->vertex == edge->opposite->vertex,
                       "incorrect next->vertex/opposite->vertex");

                int v = edge->vertex;
                my_ass(face->plane.planeside(points[v]) == 0, "vertex is not in plane");
                my_ass(edge->opposite->face->plane.planeside(points[v]) == 0,
                       "vertex is not in opposite face's plane");

                if (nocoplanar) {
                    my_ass(face->plane != edge->opposite->face->plane,
                           "should have merged faces");
                }

                edges.push_back(edge);
                edge = edge->next;
            } while (edge != face->edge);
        }
    }

    void verify_invariants() {
        verify_horizon();
        verify_active_faces();
        verify_old_faces();
        verify_eyes();
        verify_edges(true);
    }
#endif
};

#endif // FRAC_QUICKHULL3D_HPP
