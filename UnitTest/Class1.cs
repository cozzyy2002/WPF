using NUnit.Framework;
using NSubstitute;
using System;
using System.Diagnostics;

namespace UnitTest
{
    public interface IListener
    {
        void onNotify(int i);
    }

    class Caller
    {
        public Caller(IListener listener) { this.listener = listener; }
        public void call(int i) { listener.onNotify(i); }
        protected IListener listener;
    }

    class Testee : Caller
    {
        public Testee() : base(Substitute.For<IListener>())
        {
            listener
                .When(o => o.onNotify(Arg.Any<int>()))
                .Do(args => { count += (int)args[0]; });
        }
        public int count = 0;
    }

    [TestFixture]
    public class CallerTest
    {
        [Test]
        public void call([Values(3,5,0)] int i)
        {
            Testee testee = new Testee();
            testee.call(i);

            Assert.That(testee.count, Is.EqualTo(i));
        }
    }
}
