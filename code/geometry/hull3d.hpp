#ifndef QUICKHULL3D_HPP
#define QUICKHULL3D_HPP

#include "point3d.hpp" // Point3d, Plane

// *****

/**
 * 3D Quickhull for double points.
 * All degenerate cases (2+ incident, 3+ collinear, 4+ coplanar) are handled.
 *
 * Complexity: O(N log N) expected, O(N^2) worst-case
 * Reference: https://github.com/mauriciopoppe/quickhull3d
 * Tested on SPOJ-CH3D
 *
 * Usage:
 *     vector<P> points = {P(1,2,3), ...};
 *     auto hull = compute_hull3d(points);   // returns {} if all points are coplanar
 *     canonicalize_hull3d(hull);            // canonicalize if needed (B)
 *     for (const auto& face : hull) {
 *         for (int v : face) {
 *             ... // points[v]
 *         }
 *     }
 */
using P = Point3d;
struct quickhull3d {
    struct Face;
    static constexpr int VISIBLE = 0, DELETED = 1;

    struct Edge {
        Face* face = nullptr;
        Edge *next, *prev, *opposite = nullptr;
        const int vertex; // [vertex]---edge-->[next->vertex]
        explicit Edge(int v, Face* face) : face(face), vertex(v) {}
        static void marry(Edge* a, Edge* b) { a->opposite = b, b->opposite = a; }
        static void link(Edge* a, Edge* b) { a->next = b, b->prev = a; }
    };

    struct Face {
        Plane plane;
        Point3d centroid;
        Edge* edge; // an arbitrary edge in the face; a particular one initially
        int mark = VISIBLE, outside = 0, id, npoints = 3;
        explicit Face(int id) : id(id) {}
        ~Face() noexcept {
            Edge* u = edge;
            do {
                Edge* v = u->next;
                delete u;
                u = v;
            } while (u && u != edge);
        }
    };

    int N;
    vector<P> points;               // points[1..N] from input
    vector<unique_ptr<Face>> faces; // active faces list
    vector<int> eye_prev, eye_next, open;
    vector<Face*> eye_face, new_faces, old_faces;
    vector<Edge*> horizon;
    double eps = Point3d::deps;

    quickhull3d(const vector<P>& input, int skip_0 = 0)
        : N(input.size() - skip_0), points(N + 1), eye_prev(N + 1, 0), eye_next(N + 1, 0),
          open(N + 1, 0), eye_face(N + 1) {
        copy(begin(input) + skip_0, end(input), begin(points) + 1);
    }

    // Eye tables subroutines

    void link_open(int v) { open[v] = open[0], open[0] = v; }
    void link_eye(int u, int v) { eye_next[u] = v, eye_prev[v] = u; }

    void add_eye(int v, Face* face) {
        assert(v && !eye_face[v]);
        eye_face[v] = face;
        link_eye(eye_prev[face->outside], v);
        link_eye(v, face->outside);
        face->outside = v;
    }

    void remove_eye(int v) {
        assert(v && eye_face[v]);
        Face* face = eye_face[v];
        if (face->outside == v)
            face->outside = eye_next[v];

        link_eye(eye_prev[v], eye_next[v]);
        eye_face[v] = nullptr;
    }

    void remove_all_eyes(Face* face) {
        int v = face->outside, head = eye_prev[face->outside];
        while (v && eye_face[v] == face) {
            eye_face[v] = nullptr;
            link_open(v);
            v = eye_next[v]; // untouched
        }
        link_eye(head, v);
    }

    int find_furthest_eye() {
        Face* face = eye_face[eye_next[0]];
        assert(face->mark == VISIBLE && face->outside != 0);
        auto maxdist = face->plane.planedist(points[face->outside]);
        int furthest = face->outside, v = eye_next[face->outside];
        while (v && eye_face[v] == face) {
            auto dist = face->plane.planedist(points[v]);
            if (maxdist < dist) {
                maxdist = dist, furthest = v;
            }
            v = eye_next[v];
        }
        return furthest;
    }

    // Face subroutines

    /**
     * Create a new triangular face for vertices v0, v1, v2.
     * edges: 0={v0 to v1}                v2
     *        1={v1 to v2}              2    1
     *        2={v2 to v0}.          v0 -> 0 -> v1
     */
    auto add_face(int v0, int v1, int v2) {
        faces.emplace_back(make_unique<Face>(faces.size()));
        Face* face = faces.back().get();

        auto e0 = new Edge(v0, face);
        auto e1 = new Edge(v1, face);
        auto e2 = new Edge(v2, face);
        Edge::link(e0, e1), Edge::link(e1, e2), Edge::link(e2, e0);
        face->edge = e0;

        face->centroid = (points[v0] + points[v1] + points[v2]) / 3.0;
        face->plane = Plane(points[v0], points[v1], points[v2]);
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
        Face *face = edge->face, *oface = edge->opposite->face;
        return same_oriented(face->plane, oface->plane, eps) &&
               face->plane.planeside(oface->centroid, eps) >= 0 &&
               oface->plane.planeside(face->centroid, eps) >= 0;
    }

    void merge_faces(Edge* edge, bool recurse = true) {
        Face *face = edge->face, *oface = edge->opposite->face;
        Edge *a = edge->prev, *c = edge->opposite->prev;
        Edge *b = edge->next, *d = edge->opposite->next;

        // merge edge->opposite->face (with no eyes) into edge->face
        assert(oface->outside == 0 && face->mark == VISIBLE && oface->mark == VISIBLE);
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

        // Fix edges and mark face for deletion
        mark_face_for_deletion(oface);
        oface->edge = c->next;
        Edge::link(d->prev, a->next), Edge::link(b->prev, c->next); // cycle old edges
        Edge::link(a, d), Edge::link(c, b);

        // Recompute centroid
        Point3d centroid;
        face->npoints = 0, edge = face->edge;
        do {
            centroid += points[edge->vertex];
            face->npoints++, edge = edge->next;
        } while (edge != face->edge);
        face->centroid = centroid /= face->npoints;

        // Recompute normal
        Point3d normal;
        do {
            normal += centroid.cross(points[edge->vertex], points[edge->next->vertex]);
            edge = edge->next;
        } while (edge != face->edge);

        face->plane = Plane(normal, centroid);

        if (recurse && should_merge(a))
            merge_faces(a, true);
        if (recurse && should_merge(b))
            merge_faces(b, true);
    }

    void merge_new_faces() {
        vector<Face*> merged_faces; // merged + new unmerged faces
        for (Face* face : new_faces) {
            if (face->mark == VISIBLE) {
                if (should_merge(face->edge)) {
                    merged_faces.push_back(face->edge->opposite->face);
                    merge_faces(face->edge->opposite);
                } else if (should_merge(face->edge->next)) {
                    merge_faces(face->edge->next->opposite);
                } else {
                    merged_faces.push_back(face);
                }
            }
        }
        new_faces = move(merged_faces);
    }

    void final_merge_faces() {
        int H = faces.size();
        for (int i = 0; i < H; i++) {
            Face* face = faces[i].get();
            if (face->mark == VISIBLE) {
                Edge* edge = face->edge;
                do {
                    if (should_merge(edge)) {
                        merge_faces(edge->opposite);
                        break;
                    }
                    edge = edge->next;
                } while (edge != face->edge);
            }
        }
        delete_old_faces();
    }

    // Main routines

    /**
     * Compute min/max points along each axis and set epsilon based on the minimum
     * enveloping box's dimensions.
     * Create initial (maximal tetrahedral) simplex with four non-coplanar points and
     * create the four faces supporting it.
     * Set eye_face[v] for each v as the hull face seen from v that is furthest from it
     * (or leave null if none exists, meaning v is inside the hull already).
     *
     * This fails and returns false if all points are coplanar. Otherwise returns true
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
        double maxdist = P::deps;

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

        eps = 5 * P::deps * (1 + log10(max(maxdist, 1.0)));

        // select v2 such that linedist2(v2, v0, v1) is maximum (furthest from line)
        maxdist = eps;
        for (int v = 1; v <= N; v++) {
            if (v != v0 && v != v1) {
                auto dist = linedist(points[v], points[v0], points[v1]);
                if (maxdist < dist) {
                    maxdist = dist, v2 = v;
                }
            }
        }
        if (!v0 || !v1 || !v2) {
            return false;
        }

        // select v3 such that base.planedist2(v3) is maximum (furthest from plane)
        Plane base(points[v0], points[v1], points[v2]);
        maxdist = eps;
        for (int v = 1; v <= N; v++) {
            if (v != v0 && v != v1 && v != v2) {
                auto dist = base.planedist(points[v]);
                if (maxdist < dist) {
                    maxdist = dist, v3 = v;
                }
            }
        }
        if (!v0 || !v1 || !v2 || !v3) {
            return false;
        }

        if (base.signed_planedist(points[v3]) < 0) {
            make_simplex_faces(v0, v2, v1, v3);
        } else {
            make_simplex_faces(v0, v1, v2, v3);
        }

        for (int v = 1; v <= N; v++) {
            if (v != v0 && v != v1 && v != v2 && v != v3) {
                maxdist = eps;
                Face* maxface = nullptr;
                for (auto&& face : faces) {
                    auto dist = face->plane.signed_planedist(points[v]);
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

    void compute_horizon(int eye, Edge* cross, Face* face) {
        assert(eye && face && face->mark == VISIBLE && (!cross || cross->face == face));
        remove_all_eyes(face);
        mark_face_for_deletion(face);

        Edge* edge = cross ? cross->next : face->edge;
        cross = cross ? cross : face->edge;
        do {
            Edge* opposite = edge->opposite;
            if (opposite->face->mark == VISIBLE) {
                if (opposite->face->plane.planeside(points[eye], eps) == 1) {
                    compute_horizon(eye, opposite, opposite->face);
                } else {
                    horizon.push_back(edge);
                }
            }
            edge = edge->next;
        } while (edge != cross);
    }

    void resolve_open_points() {
        for (int v = open[0]; v; v = open[v]) {
            auto maxdist = eps;
            Face* maxface = nullptr;
            for (Face* face : new_faces) {
                auto dist = face->plane.signed_planedist(points[v]);
                if (maxdist < dist) {
                    maxdist = dist, maxface = face;
                }
            }
            if (maxface) {
                add_eye(v, maxface);
            }
        }
        open[0] = 0;
        new_faces.clear();
    }

    void add_vertex_to_hull(int eye) {
        Face* face = eye_face[eye];
        remove_eye(eye);
        compute_horizon(eye, nullptr, face);
        add_horizon_faces(eye);
        merge_new_faces();
        resolve_open_points();
        delete_old_faces();
    }

    void extend_simplex() {
        while (eye_next[0]) {
            int eye = find_furthest_eye();
            add_vertex_to_hull(eye);
        }
    }

    // Interface

    bool compute() {
        bool ok = initialize_simplex();
        if (ok) {
            extend_simplex();
            final_merge_faces();
        }
        return ok;
    }

    auto extract_hull(int skip_0 = 0) const {
        vector<vector<int>> hull(faces.size());
        for (auto&& face : faces) {
            Edge* edge = face->edge;
            do {
                int v = edge->vertex - 1 + skip_0;
                hull[face->id].push_back(v);
                edge = edge->next;
            } while (edge != face->edge);
        }
        return hull;
    }
};

using hull_t = vector<vector<int>>;

void simplify_hull(hull_t& hull, const vector<P>& points, double eps) {
    // Remove collinear points from faces.
    for (auto& face : hull) {
        vector<int> filtered_face;
        for (int j = 0, N = face.size(); j < N; j++) {
            int i = (j + N - 1) % N, k = (j + 1) % N;
            int u = face[i], v = face[j], w = face[k];
            if (!collinear(points[u], points[v], points[w], eps))
                filtered_face.push_back(face[j]);
        }
        face = move(filtered_face);
    }
}

auto compute_hull(const vector<P>& points, int skip_0 = 0) {
    quickhull3d qh(points, skip_0);
    qh.compute();
    auto hull = qh.extract_hull(skip_0);
    simplify_hull(hull, points, qh.eps);
    return hull;
}

#endif // QUICKHULL3D_HPP
