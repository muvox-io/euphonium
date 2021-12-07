import Icon from "../Icon";
import { route } from 'preact-router';
import Button from "../Button";

export default ({ title = "", subtitle = "", children = {}, enableButton = true }) => {
  return (
    <div class='p-8 relative'>
      {enableButton ? <div onClick={() => route('/web')} class='md:hidden shadow-m absolute text-app-text-secondary right-8 top-8 w-12 h-12 bg-app-secondary rounded-full items-center justify-center flex'>
        <Icon name='close' />
      </div> : null }
      <div class='text-3xl'>{title}</div>
      <div class='text-m mt-2 text-app-text-secondary'>{subtitle}</div>
      <div class='md:shadow-lg md:bg-app-primary md:p-6 md:rounded-2xl mt-5 mb-15'>
        {children}
      </div>
    </div>
  );
};
