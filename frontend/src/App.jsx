import { useState, useEffect } from 'react'

function App() {
  const [posts, setPosts] = useState([])
  const [projects, setProjects] = useState([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState(null)

  useEffect(() => {
    const fetchData = async () => {
      try {
        // Fetch posts
        const postsRes = await fetch('http://localhost:8080/api/posts')
        if (!postsRes.ok) throw new Error('Failed to fetch posts')
        const postsData = await postsRes.json()
        setPosts(postsData)

        // Fetch projects
        const projectsRes = await fetch('http://localhost:8080/api/projects')
        if (!projectsRes.ok) throw new Error('Failed to fetch projects')
        const projectsData = await projectsRes.json()
        setProjects(projectsData)
      } catch (err) {
        setError(err.message)
      } finally {
        setLoading(false)
      }
    }

    fetchData()
  }, [])

  if (loading) return <div className="flex justify-center items-center h-screen text-xl">Loading C Backend...</div>
  if (error) return <div className="flex justify-center items-center h-screen text-red-500 text-xl">Error: {error}. Is the C server running?</div>

  return (
    <div className="min-h-screen p-8 font-sans">
      <header className="mb-12 text-center">
        <h1 className="text-4xl font-bold text-gray-800 mb-2">My C + React Portfolio</h1>
        <p className="text-gray-600">Powered by a C Backend & MySQL Database</p>
      </header>

      <main className="max-w-6xl mx-auto grid grid-cols-1 md:grid-cols-2 gap-12">

        {/* Blog Section */}
        <section>
          <h2 className="text-2xl font-bold text-gray-700 mb-6 border-b pb-2">Recent Posts</h2>
          <div className="space-y-6">
            {posts.map(post => (
              <div key={post.id} className="bg-white p-6 rounded-lg shadow-md hover:shadow-lg transition">
                <h3 className="text-xl font-semibold mb-2">{post.title}</h3>
                <p className="text-gray-600 mb-4">{post.content}</p>
                <div className="text-sm text-gray-400">{new Date(post.created_at).toLocaleDateString()}</div>
              </div>
            ))}
          </div>
        </section>

        {/* Projects Section */}
        <section>
          <h2 className="text-2xl font-bold text-gray-700 mb-6 border-b pb-2">Projects</h2>
          <div className="grid grid-cols-1 gap-6">
            {projects.map(project => (
              <div key={project.id} className="bg-white p-6 rounded-lg shadow-md hover:shadow-lg transition flex flex-col items-center text-center">
                <img src={project.image_url} alt={project.title} className="w-full h-48 object-cover rounded mb-4 bg-gray-200" />
                <h3 className="text-xl font-semibold mb-2">{project.title}</h3>
                <p className="text-gray-600 mb-4">{project.description}</p>
                <a href={project.url} className="text-blue-500 hover:underline mt-auto">View Project &rarr;</a>
              </div>
            ))}
          </div>
        </section>

      </main>

      <footer className="mt-20 text-center text-gray-500 text-sm">
        &copy; 2026 My Portfolio. Built with C, React, and MySQL.
      </footer>
    </div>
  )
}

export default App
