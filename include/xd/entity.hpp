#ifndef H_XD_ENTITY
#define H_XD_ENTITY

#include <xd/detail/entity.hpp>

#include <xd/handle.hpp>
#include <xd/event_bus.hpp>
#include <boost/config.hpp>
#include <boost/any.hpp>
#include <unordered_map>
#include <functional>
#include <memory>
#include <list>
#include <map>
#include <type_traits>

#ifdef BOOST_NO_VARIADIC_TEMPLATES
#include <boost/preprocessor/iteration/iterate.hpp>
#endif

namespace xd
{
	template <typename T>
	class logic_component : public detail::logic_component<T>
	{
	};

	template <typename T>
	class render_component : public detail::render_component<T>
	{
	};
    
	template <typename T>
	class component : public detail::component<T>
	{
	};
	
	class entity_base
	{
	protected:
		~entity_base() {}
	};
    
	template <typename Base = entity_base>
	class entity : public Base
	{
	public:
		// required for xd::factory
		typedef xd::handle<entity> handle;
		typedef xd::weak_handle<entity> weak_handle;
		// component handle typedefs
		typedef typename xd::component<entity<Base>>::handle component_handle;
		typedef typename xd::logic_component<entity<Base>>::handle logic_component_handle;
		typedef typename xd::render_component<entity<Base>>::handle render_component_handle;

		entity()
		{
		}

#ifndef BOOST_NO_VARIADIC_TEMPLATES
		// constructor that delegates parameters to the entity_base
		template <typename... Args>
		entity(Args&&... args)
			: Base(std::forward<Args>(args)...)
		{
		}
#else
		// generate xd::entity::entity overloads with file iteration (up to XD_MAX_ARITY parameters)
		#define BOOST_PP_ITERATION_PARAMS_1 (3, (1, XD_MAX_ARITY, <xd/detail/iterate_entity_constructor.hpp>))
		#include BOOST_PP_ITERATE()
#endif

		entity(const Base& base)
			: Base(base)
		{
		}

		virtual ~entity()
		{
		}

		template <typename T>
		T& get()
		{
			std::size_t hash = typeid(T).hash_code();
			auto i = m_type_to_data.find(hash);
			if (i == m_type_to_data.end())
				i = m_type_to_data.insert(std::make_pair(hash, T())).first;
			return *boost::any_cast<T>(&i->second);
		}

		template <typename T>
		T& get(const std::string& key)
		{
			auto i = m_key_to_data.find(key);
			if (i == m_key_to_data.end())
				i = m_key_to_data.insert(std::make_pair(key, T())).first;
			return *boost::any_cast<T>(&i->second);
		}

		template <typename T>
		bool has()
		{
			std::size_t hash = typeid(T).hash_code();
			return m_type_to_data.find(hash) != m_type_to_data.end();
		}

		template <typename T>
		bool has(const std::string& key)
		{
			return m_key_to_data.find(key) != m_key_to_data.end();
		}

		template <typename T>
		void on(const std::string& name, std::function<bool (const T&)> callback, std::function<bool (const T&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(callback);
		}

		template <typename T>
		void on(const std::string& name, bool (*callback)(const T&), std::function<bool (const typename detail::identity<T>::type&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(callback);
		}

		template <typename T, typename C>
		void on(const std::string& name, bool (C::*callback)(const T&), C *obj, std::function<bool (const typename detail::identity<T>::type&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(std::bind(callback, obj, std::placeholders::_1));
		}

		template <typename T, typename C>
		void on(const std::string& name, bool (C::*callback)(const T&) const, C *obj, std::function<bool (const typename detail::identity<T>::type&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(std::bind(callback, obj, std::placeholders::_1));
		}

		template <typename T>
		void on(const std::string& name, T obj)
		{
			on(name, &T::operator(), &obj);
			//get_event_bus<typename detail::first_argument<T>::type>()[name].add(obj);
		}

		template <typename T, typename U>
		void on(const std::string& name, T obj, U filter)
		{
			on(name, &T::operator(), &obj, filter);
			//get_event_bus<typename detail::first_argument<T>::type>()[name].add(obj, filter);
		}
        
		template <typename T>
		void trigger(const std::string& name, const T& args)
		{
			get_event_bus<T>()[name](args);
		}
        
		void add_component(const logic_component_handle& component, int priority = 0)
		{
			m_components[priority].logic_components.push_back(component);
			component->init(*this);
		}
        
		void add_component(const render_component_handle& component, int priority = 0)
		{
			m_components[priority].render_components.push_back(component);
			component->init(*this);
		}
        
		void add_component(const component_handle& component, int priority = 0)
		{
			m_components[priority].logic_components.push_back(component);
			m_components[priority].render_components.push_back(component);
			component->init(*this);
		}

		void del_component(const logic_component_handle& component, int priority)
		{
			logic_component_list_t& components = m_components[priority].logic_components;
			auto i = components.find(component);
			if (i != components.end()) {
				components.erase(i);
			}
		}

		void del_component(const logic_component_handle& component)
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				del_component(component, i->first);
			}
		}

		void del_component(const render_component_handle& component, int priority)
		{
			logic_component_list_t& components = m_components[priority].logic_components;
			auto i = components.find(component);
			if (i != components.end()) {
				components.erase(i);
			}
		}

		void del_component(const render_component_handle& component)
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				del_component(component, i->first);
			}
		}

		void del_component(const component_handle& component, int priority)
		{
			components_set& components = m_components[priority];
			{
				auto i = components.logic_components.find(component);
				if (i != components.end()) {
					components.erase(i);
				}
			}
			{
				auto i = components.render_components.find(component);
				if (i != components.end()) {
					components.erase(i);
				}
			}
		}

		void del_component(const component_handle& component)
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				del_component(component, i->first);
			}
		}

		void clear_components()
		{
			m_components.clear();
		}
        
		void update()
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				for (auto j = i->second.logic_components.begin(); j != i->second.logic_components.end(); ++j) {
					(*j)->update(*this);
				}
			}
		}

		void render()
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				for (auto j = i->second.render_components.begin(); j != i->second.render_components.end(); ++j) {
					(*j)->render(*this);
				}
			}
		}
        
	private:
		typedef std::list<logic_component_handle> logic_component_list_t;
		typedef std::list<render_component_handle> render_component_list_t;

		// data
		std::unordered_map<std::size_t, boost::any> m_type_to_data;
		std::unordered_map<std::string, boost::any> m_key_to_data;

		// internal struct to hold component lists per priority
		struct components_set
		{
			logic_component_list_t logic_components;
			render_component_list_t render_components;
		};

		// we have a list of logic and render components per priority
		std::map<int, components_set> m_components;
        
		// the bound events
		std::unordered_map<std::size_t, boost::any> m_events;
        
		// utility function to return event_bus for given arg type
		template <typename T>
		event_bus<T>& get_event_bus()
		{
			// calculate hash of the argument type
			std::size_t hash = typeid(T).hash_code();
			// find from the events
			auto i = m_events.find(hash);
			if (i == m_events.end()) {
				// not found, insert an empty event_bus
				m_events[hash] = event_bus<T>();
			}
			// return the event bus
			return *boost::any_cast<event_bus<T>>(&m_events[hash]);
		}
	};
}

#endif
