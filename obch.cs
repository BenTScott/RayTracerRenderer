using System;
using System.Linq;
using System.Runtime.Caching;
using System.Threading.Tasks;
using NES.Turas.People.Utilities;
using NES.Turas.People.Utilities.Guard;
using Serilog;

namespace NES.Turas.People.Service.Caching
{
    /// <summary>
    /// Implementation of ICacheProvider using ObjectCache as backing store
    /// </summary>
    public class ObjectCacheProvider : ICacheProvider
    {
        private static readonly ILogger Logger = Log.ForContext<ObjectCacheProvider>();

        // You can either do this, or wrap every single object in a cache wrapper, this seem simpler. It's a random GUID, should be unique enough.
        private const string CacheNullValue = "7bdb4c3e-5a77-4448-89ed-9d4a9ebb6032";

        // Default TTL of 10mins if not specified
        private readonly CacheItemPolicy _defaultPolicy =
            new CacheItemPolicy
            {
                AbsoluteExpiration =
                    DateTime.Now.Add(TimeSpan.FromSeconds(
                        PeopleApplicationSettings.GetWithFallback(AppSettingKeys.DefaultCacheTtl, 1200)))
            };

        private readonly ObjectCache _cache;

        /// <summary>
        /// New ObjectCacheProvider using MemoryCache.Default as backing store
        /// </summary>
        public ObjectCacheProvider()
        {
            _cache = MemoryCache.Default;
        }

        public ObjectCacheProvider(ObjectCache cache)
        {
            _cache = cache.ThrowIfNull(nameof(cache));
        }

        /// <summary>
        /// Checks if the specified key exists within the cache..
        /// </summary>
        /// <param name="key">The key.</param>
        /// <returns>true if item exists, else false</returns>
        public bool Exists(string key)
        {
            key.ThrowIfNullOrWhitespace(nameof(key));
            // NOTE(DG): MemoryCache is threadsafe but Contains can result in cache miss https://stackoverflow.com/a/40089949/83096
            lock (_cache)
            {
                return _cache.Contains(key);
            }
        }

        /// <summary>
        /// Gets the specified key.
        /// </summary>
        /// <typeparam name="T">Type of object</typeparam>
        /// <param name="key">The key.</param>
        /// <returns>CacheValue representing object and success</returns>
        public CacheValue<T> Get<T>(string key)
            where T : class
        {
            key.ThrowIfNullOrWhitespace(nameof(key));
            var success = TryGet(key, out T value);
            return new CacheValue<T>(value, success);
        }

        /// <summary>
        /// Removes the item with the specified key from the cache.
        /// </summary>
        /// <param name="key">The key to remove.</param>
        public void Remove(params string[] key)
        {
            key.ThrowIfNull(nameof(key));
            foreach (var cacheKey in key.Where(k => !string.IsNullOrWhiteSpace(k)))
            {
                lock (_cache)
                {
                    _cache.Remove(cacheKey);
                }
            }
        }

        /// <summary>
        /// Fetches item with specified key, or uses Func to fetch and insert to cache before returning
        /// </summary>
        public CacheValue<T> GetOrInsert<T>(string key, Func<T> getData)
            => GetOrInsertInternal(key, getData, _defaultPolicy);
        
        public CacheValue<T> GetOrInsert<T>(string key, Func<T> getData, TimeSpan timeToLive)
        {
            var policy = new CacheItemPolicy { AbsoluteExpiration = DateTime.Now.Add(timeToLive) };
            return GetOrInsertInternal(key, getData, policy);
        }

        public Task<CacheValue<T>> GetOrInsertAsync<T>(string key, Func<Task<T>> getData)
            => GetOrInsertAsyncInternal(key, getData, _defaultPolicy);

        public Task<CacheValue<T>> GetOrInsertAsync<T>(string key, Func<Task<T>> getData, TimeSpan timeToLive)
        {
            var policy = new CacheItemPolicy { AbsoluteExpiration = DateTime.Now.Add(timeToLive) };
            return GetOrInsertAsyncInternal(key, getData, policy);
        }

        public void Insert<T>(string key, T value)
            => InsertInternal(key, value, _defaultPolicy);

        public void Insert<T>(string key, T value, TimeSpan timeToLive)
        {
            var policy = new CacheItemPolicy { AbsoluteExpiration = DateTime.Now.Add(timeToLive) };
            InsertInternal(key, value, policy);
        }

        public void Upsert<T>(string key, T value)
            => UpsertInternal(key, value, _defaultPolicy);

        public void Upsert<T>(string key, T value, TimeSpan timeToLive)
        {
            var policy = new CacheItemPolicy { AbsoluteExpiration = DateTime.Now.Add(timeToLive) };
            UpsertInternal(key, value, policy);
        }

        private object Get(string key)
        {
            key.ThrowIfNullOrWhitespace(nameof(key));
            // NOTE(DG): MemoryCache is threadsafe but Get can result in cache miss https://stackoverflow.com/a/40089949/83096
            lock (_cache)
            {
                var item = _cache.Get(key);
                // This handles the lack of null support in MemoryCache
                return CacheNullValue.Equals(item) ? null : item;
            }
        }

        private CacheValue<T> GetOrInsertInternal<T>(string key, Func<T> getData, CacheItemPolicy policy)
        {
            key.ThrowIfNullOrWhitespace(nameof(key));

            // Try and get the value from cache
            var success = TryGetValue(key, out T value);

            // Data found, return value
            if (success)
            {
                Logger.Verbose("Got data with key {key}", key);
                return new CacheValue<T>(value, true);
            }

            if (getData == null)
            {
                return CacheValue<T>.Failure;
            }

            // Didn't get it from the cache, load it now
            value = getData();

            InsertInternal(key, value, policy);

            return new CacheValue<T>(value, true);
        }

        private async Task<CacheValue<T>> GetOrInsertAsyncInternal<T>(string key, Func<Task<T>> getData, CacheItemPolicy policy)
        {
            key.ThrowIfNullOrWhitespace(nameof(key));

            var success = TryGetValue(key, out T value);

            if (success)
            {
                return new CacheValue<T>(await UnwrapTask<T>(value), true);
            }

            if (getData == null)
            {
                return CacheValue<T>.Failure;
            }

            try
            {
                var result = getData();

                if (result.IsCanceled || result.IsFaulted)
                {
                    lock (_cache)
                    {
                        _cache.Remove(key);
                    }
                    return CacheValue<T>.Failure;
                }

                var factoryResult = await result;
                InsertInternal(key, factoryResult, policy);

                return new CacheValue<T>(factoryResult, true);
            }
            catch (Exception ex)
            {
                // Don't cache the error
                lock (_cache)
                    _cache.Remove(key);

                Logger.Warning(ex, "Error getting async data with key {key} to insert to cache", key);

                return CacheValue<T>.Failure;
            }
        }

        private void InsertInternal<T>(string key, T value, CacheItemPolicy policy)
        {
            key.ThrowIfNullOrWhitespace(nameof(key));

            Logger.Verbose("Adding item {key}, value {value}", key, value);
            var cacheValue = value == null ? (object)CacheNullValue : value;
            lock (_cache)
            {
                _cache.Add(key, cacheValue, policy);
            }
        }

        private void UpsertInternal<T>(string key, T value, CacheItemPolicy policy)
        {
            key.ThrowIfNullOrWhitespace(nameof(key));

            if (TryGet(key, out T val))
            {
                lock (_cache)
                {
                    _cache.Remove(key);
                }
            }

            InsertInternal(key, value, policy);
        }

        private bool TryGetValue<T>(string key, out T value)
        {
            // Try and get the value from cache
            var success = TryGet(key, out value);
            if (!(value is Task) || !(value as Task).IsFaulted) return success;

            // The cached value is a faulted task. If so pretend nothing was there.
            Logger.Warning("Object from cache with key {key} was a faulted task", key);
            return false;
        }

        private bool TryGet<T>(string key, out T value)
        {
            Logger.Verbose("Fetching item with key {key}", key);
            var returnVal = Get(key);
            if (returnVal == null)
            {
                Logger.Verbose("Got null for key {key}", key);
                // Item with key doesn't exist or is null, return default for type
                value = default(T);

                // value might actually have been stored as null, so return whether the key exists
                return Exists(key);
            }

            try
            {
                Logger.Verbose("Got data with key {key}", key);
                // Success, return item
                value = (T)returnVal;
                return true;
            }
            catch (InvalidCastException ex)
            {
                // Log error and return default for type
                Logger.Error(ex, "Error getting item with key {key} from cache. Attempted to cast as {type}", key, typeof(T));
                value = default(T);
                return false;
            }
        }

        private static async Task<T> UnwrapTask<T>(object item)
        {
            if (item is Task<T> task)
                return await task;

            if (item is T variable)
                return variable;

            return default(T);
        }
    }
}